#include "./cad_document.h"

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
