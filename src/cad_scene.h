#pragma once

#include "./crosshair_item.h"
#include "./cad_document/cad_document.h"
#include <QGraphicsScene>

class CadToolManager;
class QGraphicsSceneEvent;
class QGraphicsItem;

class CadScene : public QGraphicsScene
{
    Q_OBJECT
public:
    explicit CadScene(CadToolManager* toolManager, QObject* parent = nullptr);
    ~CadScene();

    void setDocument(CadDocument* document);
    CadDocument* getDocument() const { return m_document; }

signals:
    void cursorPositionChanged(const QPointF& position);

protected:
    void mousePressEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseMoveEvent(QGraphicsSceneMouseEvent* event) override;
    void mouseReleaseEvent(QGraphicsSceneMouseEvent* event) override;
    void keyPressEvent(QKeyEvent* event) override;

private:
    CrosshairItem* m_crosshair = nullptr;
    CadToolManager* m_toolManager = nullptr;
    QGraphicsLineItem* m_centerHLine = nullptr;
    QGraphicsLineItem* m_centerVLine = nullptr;

    QPen* m_dashDotDotPenRed = nullptr;
    QPen* m_dotPenRed = nullptr;

    CadDocument* m_document = nullptr;
};
