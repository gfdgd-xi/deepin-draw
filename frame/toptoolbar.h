#ifndef TOPTOOLBAR_H
#define TOPTOOLBAR_H

#include "drawshape/globaldefine.h"

#include <DFrame>
#include <DMenu>
#include <DLabel>

#include <DWidget>
#include <DStackedWidget>
#include <DPushButton>
#include <QKeyEvent>


DWIDGET_USE_NAMESPACE

class CommonshapeWidget;
class PolygonalStarAttributeWidget;
class PolygonAttributeWidget;
class CCutWidget;
class CPenWidget;
class LineWidget;
class CPictureWidget;
class TextWidget;
class BlurWidget;
class AdjustsizeWidget;

class ArrowRectangle;
class ColorPanel;

class TopToolbar : public DFrame
{
    Q_OBJECT

public:
    TopToolbar(DWidget *parent = nullptr);
    ~TopToolbar();

    DMenu *mainMenu();

signals:
    void generateSaveImage();
    void printImage();
    void importPicBtnClicked();
    void signalAttributeChanged();
    //传递图片的旋转和翻转信号
    void signalPassPictureOperation(int);
    //放大缩小信号
    void signalZoom(qreal);
    void signalShowExportDialog();


public:
//    void showDrawDialog();
    void showSaveDialog();
    void showPrintDialog();
    EDrawToolMode middleWidgetStatus();

public slots:
    void updateMiddleWidget(int type);
    void showColorfulPanel(DrawStatus drawstatus, QPoint pos, bool visible = true);
    void updateColorPanelVisible(QPoint pos);
    /**
     * @brief slotChangeAttributeFromScene 根据选中的图元更新属性栏
     * @param flag
     * @param primitiveType
     */
    void slotChangeAttributeFromScene(bool flag, int primitiveType);
    void slotZoom(const QString &scale);
    void slotSetScale(const qreal scale);

protected:
    void resizeEvent(QResizeEvent *event) Q_DECL_OVERRIDE;

private:
    QString m_path;
    QStringList m_paths;
    DStackedWidget *m_stackWidget;
    int m_textFontsize = 12;

    DWidget *m_emptyWidget;
    CommonshapeWidget *m_commonShapeWidget;
    PolygonalStarAttributeWidget *m_polygonalStarWidget;
    PolygonAttributeWidget *m_PolygonWidget;
    CCutWidget *m_cutWidget;
    CPictureWidget *m_picWidget;
    LineWidget *m_drawLineWidget;
    TextWidget *m_drawTextWidget;
    BlurWidget *m_drawBlurWidget;
    CPenWidget *m_penWidget;
//    AdjustsizeWidget *m_adjustsizeWidget;

    ArrowRectangle *m_colorARect;
    ColorPanel *m_colorPanel;
    DrawStatus  m_drawStatus;
    DComboBox  *m_scaleComboBox;
    DMenu *m_mainMenu;


private:
    void initUI();
    void initConnection();
    void initStackWidget();
    void initMenu();
    void initComboBox();
};

#endif // TOPTOOLBAR_H
