#pragma once

#include <QObject>
#include <vector>
#include <memory>
#include "./cad_entity.h"

class CadDocument : public QObject {
    Q_OBJECT
public:
    explicit CadDocument(QObject* parent = nullptr);

    // Fügt ein Objekt dem Dokument hinzu (Ownership geht ans Dokument über)
    CadEntity* addEntity(std::unique_ptr<CadEntity> entity);

    // Entfernt ein Objekt aus dem Dokument
    bool removeEntity(CadEntity* entity);

    // Zugriff auf alle Objekte (z. B. für DXF-Export)
    const std::vector<std::unique_ptr<CadEntity>>& getEntities() const { return m_entities; }

    // Löscht alle Objekte aus dem Dokument
    void clear();

    bool saveToFile(const QString& filePath) const;
    bool loadFromFile(const QString& filePath);

signals:
    void entityAdded(CadEntity* entity);
    void entityRemoved(CadEntity* entity);
    void documentCleared();

private:
    std::vector<std::unique_ptr<CadEntity>> m_entities;
};
