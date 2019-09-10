#include "cselecttool.h"
#include "cdrawscene.h"
#include "cgraphicsitem.h"
#include "cgraphicslineitem.h"
#include <QGraphicsSceneMouseEvent>
#include <QDebug>
#include <QCursor>
#include <QtMath>
#include <QGraphicsTextItem>
#include <QTextCursor>
CSelectTool::CSelectTool ()
    : IDrawTool (selection)
    , m_currentSelectItem(nullptr)
    , m_dragHandle(CSizeHandleRect::None)
    , m_bRotateAng(false)
    , m_rotateAng(0)
{

}

CSelectTool::~CSelectTool()
{

}

void CSelectTool::mousePressEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        m_bMousePress = true;
        m_sPointPress = event->scenePos();
        //选中图元

        if (CSizeHandleRect::None == m_dragHandle) {
            //要先触发scene->mouseEvent(event);  才能获取是否有图元被选中
            scene->mouseEvent(event);

            QList<QGraphicsItem *> items = scene->selectedItems();

            if ( items.count() != 0 ) {
                QGraphicsItem *item = items.first();
                //需要区别图元或文字
                if (item->type() != TextType) {
                    m_currentSelectItem = static_cast<CGraphicsItem *>(item);
                    scene->changeAttribute(true, item);
                } else {
                    m_currentSelectItem = nullptr;
                }
                //scene->changeAttribute(true, m_currentSelectItem->pen(), m_currentSelectItem->brush());
            } else {
                m_currentSelectItem = nullptr;
            }
        }
    } else {
        scene->mouseEvent(event);
    }
}

void CSelectTool::mouseMoveEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    // 再判断一次
    QList<QGraphicsItem *> items = scene->selectedItems();
    //if (m_currentSelectItem == nullptr) {

    if ( items.count() != 0 ) {
        QGraphicsItem *item = items.first();
        CGraphicsItem *currentItem = static_cast<CGraphicsItem *>(item);
        if (currentItem != m_currentSelectItem) {
            //需要区别图元或文字
            if (item->type() != TextType) {
                m_currentSelectItem = currentItem;
                m_rotateAng = m_currentSelectItem->rotation();
                scene->changeAttribute(true, item);
            }
        }
    }
    // }
    //碰撞检测
    if (items.count() != 0 && nullptr != m_currentSelectItem && !m_bMousePress) {
        CSizeHandleRect::EDirection dragHandle = m_currentSelectItem->hitTest(event->scenePos());

        if (dragHandle != m_dragHandle) {
            m_dragHandle = dragHandle;
            scene->setCursor(QCursor(m_currentSelectItem->getCursor(m_dragHandle)));
            m_rotateAng = m_currentSelectItem->rotation();
        }
    }

    if ( m_bMousePress ) {
        if (m_dragHandle != CSizeHandleRect::None && m_dragHandle != CSizeHandleRect::Rotation) {
            m_currentSelectItem->resizeTo(m_dragHandle, event->scenePos());
        } else if (m_dragHandle == CSizeHandleRect::Rotation) {
            //旋转图形 有BUG
            m_bRotateAng = true;
            QPointF center = m_currentSelectItem->rect().center();
            m_currentSelectItem->setTransformOriginPoint(center);
            QPointF mousePoint = event->scenePos();
            QPointF centerToScence = m_currentSelectItem->mapToScene(center);
            qreal len_y = mousePoint.y() - centerToScence.y();
            qreal len_x = mousePoint.x() - centerToScence.x();
            qreal angle = atan2(-len_x, len_y) * 180 / M_PI + 180;
            qDebug() << "angle" << angle << endl;
            if ( angle > 360 ) {
                angle -= 360;
            }

            m_currentSelectItem->setRotation(angle);

//            scene->sendRotateSignal(m_currentSelectItem, angle);

        } else {
            scene->mouseEvent(event);
        }
    } else {
        scene->mouseEvent(event);
    }
}

void CSelectTool::mouseReleaseEvent(QGraphicsSceneMouseEvent *event, CDrawScene *scene)
{
    if (event->button() == Qt::LeftButton) {
        m_bMousePress = false;
        m_sPointRelease = event->scenePos();

        if (m_currentSelectItem != nullptr) {
            if (m_bRotateAng) {
                emit scene->itemRotate(m_currentSelectItem, m_rotateAng);
            } else {
                emit scene->itemMoved(m_currentSelectItem, m_sPointRelease - m_sPointPress );
            }
        }
    }
    scene->mouseEvent(event);
}


