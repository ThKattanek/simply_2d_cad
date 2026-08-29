#include "./cad_document.h"
#include "cad_line.h"
#include "cad_point.h"

#include <QFile>

// Magic Number identifiziert deine Datei eindeutig als CAD-Datei
constexpr quint32 CAD_MAGIC_NUMBER = 0x53324443; // "S2DC" in Hex
constexpr quint32 CAD_FILE_VERSION = 1;

CadDocument::CadDocument(QObject* parent) : QObject(parent) {}

CadEntity* CadDocument::addEntity(std::unique_ptr<CadEntity> entity) {
    if (!entity) return nullptr;

    CadEntity* rawPtr = entity.get();
    m_entities.push_back(std::move(entity));

    emit entityAdded(rawPtr);
    return rawPtr;
}

bool CadDocument::removeEntity(CadEntity* entity) {
    auto it = std::find_if(m_entities.begin(), m_entities.end(),
                           [entity](const std::unique_ptr<CadEntity>& e) { return e.get() == entity; });

    if (it != m_entities.end()) {
        emit entityRemoved(entity);
        m_entities.erase(it); // Löscht das C++ Objekt und gibt den Speicher frei
        return true;
    }
    return false;
}

void CadDocument::clear() {
    m_entities.clear();
    emit documentCleared();
}

bool CadDocument::saveToFile(const QString &filePath) const
{
    QFile file(filePath);
    if (!file.open(QIODevice::WriteOnly)) {
        return false;
    }

    QDataStream stream(&file);
    // Erzeugt ein einheitliches Binärformat unabhängig vom Betriebssystem (Big/Little Endian)
    stream.setByteOrder(QDataStream::BigEndian);

    // 1. Header schreiben
    stream << CAD_MAGIC_NUMBER;
    stream << CAD_FILE_VERSION;

    // 2. Anzahl der Elemente schreiben
    stream << static_cast<quint32>(m_entities.size());

    // 3. Jedes Element binär schreiben
    for (const auto& entity : m_entities) {
        entity->serialize(stream);
    }

    return true;
}

bool CadDocument::loadFromFile(const QString &filePath)
{
    QFile file(filePath);
    if (!file.open(QIODevice::ReadOnly)) {
        return false;
    }

    QDataStream stream(&file);
    stream.setByteOrder(QDataStream::BigEndian);

    // 1. Header prüfen
    quint32 magic;
    stream >> magic;
    if (magic != CAD_MAGIC_NUMBER) {
        return false; // Keine gültige CAD-Datei!
    }

    quint32 version;
    stream >> version;
    if (version > CAD_FILE_VERSION) {
        return false; // Datei wurde mit einer neueren Programmversion erstellt
    }

    // Szene & Dokument leeren vor dem Laden
    clear();

    // 2. Anzahl der Elemente lesen
    quint32 count;
    stream >> count;

    // 3. Elemente einlesen und über Factory erzeugen
    for (quint32 i = 0; i < count; ++i) {
        quint8 typeValue;
        stream >> typeValue;
        auto type = static_cast<EntityType>(typeValue);

        std::unique_ptr<CadEntity> newEntity = nullptr;

        switch (type) {
        case EntityType::Line:
            newEntity = std::make_unique<CadLine>(QPointF(), QPointF());
            break;
        case EntityType::Point:
            newEntity = std::make_unique<CadPoint>(QPointF());
            break;
        default:
            return false; // Unbekannter Typ / Beschädigte Datei
        }

        if (newEntity) {
            newEntity->deserialize(stream);
            // Übergabe ans Dokument -> Signal löst automatisches Zeichnen in CadScene aus!
            addEntity(std::move(newEntity));
        }
    }

    return true;
}
