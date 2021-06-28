/*
 * Copyright (C) 2020 ~ 2021 Uniontech Software Technology Co.,Ltd.
 *
 * Author:     Ji XiangLong <jixianglong@uniontech.com>
 *
 * Maintainer: WangYu <wangyu@uniontech.com>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/
#include "ctexttool.h"
#include "cdrawscene.h"
#include "cgraphicstextitem.h"
#include "cdrawparamsigleton.h"
#include "widgets/ctextedit.h"
#include "frame/cgraphicsview.h"
#include "cundoredocommand.h"
#include "cattributeitemwidget.h"
#include "cattributemanagerwgt.h"
#include "application.h"
#include "cviewmanagement.h"
#include "cundoredocommand.h"

#include <DFontSizeManager>

#include <QLineEdit>
#include <QFontComboBox>
#include <QAbstractItemView>

#include <QGraphicsView>
#include <DToolButton>

CTextTool::CTextTool()
    : IDrawTool(text)
{

}

CTextTool::~CTextTool()
{

}

DrawAttribution::SAttrisList CTextTool::attributions()
{
    DrawAttribution::SAttrisList result;
    result << defaultAttriVar(DrawAttribution::EFontColor)
           << defaultAttriVar(DrawAttribution::EFontFamily)
           << defaultAttriVar(DrawAttribution::EFontWeightStyle)
           << defaultAttriVar(DrawAttribution::EFontSize);
    return result;
}

void CTextTool::registerAttributionWidgets()
{
    //1.安装文字颜色设置控件
    auto fontColor = new CColorSettingButton(tr("Color"), nullptr, false);
    fontColor->setAttribution(EFontColor);
    CAttributeManagerWgt::installComAttributeWgt(EFontColor, fontColor, QColor(0, 0, 0));
    drawApp->setWidgetAccesibleName(fontColor, "Text color button");

    connect(fontColor, &CColorSettingButton::colorChanged, this, [ = ](const QColor & color, int phase) {
        CCmdBlock block(isTextEnableUndoThisTime() ? drawApp->currentDrawScence()->selectGroup() : nullptr);
        emit drawApp->attributionsWgt()->attributionChanged(EFontColor, color, phase, false);
    });


    //2.安装字体字重设置控件
    initFontWeightWidget();

    //3.安装字体族设置控件
    initFontFamilyWidget(m_fontHeavy);

    //4.安装字体大小设置控件
    initFontFontSizeWidget();
}

QAbstractButton *CTextTool::initToolButton()
{
    DToolButton *m_textBtn = new DToolButton;
    m_textBtn->setShortcut(QKeySequence(QKeySequence(Qt::Key_T)));
    drawApp->setWidgetAccesibleName(m_textBtn, "Text tool button");
    m_textBtn->setToolTip(tr("Text(T)"));
    m_textBtn->setIconSize(QSize(48, 48));
    m_textBtn->setFixedSize(QSize(37, 37));
    m_textBtn->setCheckable(true);

    connect(m_textBtn, &DToolButton::toggled, m_textBtn, [ = ](bool b) {
        QIcon icon       = QIcon::fromTheme("ddc_text tool_normal");
        QIcon activeIcon = QIcon::fromTheme("ddc_text tool_active");
        m_textBtn->setIcon(b ? activeIcon : icon);
    });
    m_textBtn->setIcon(QIcon::fromTheme("ddc_text tool_normal"));
    return m_textBtn;
}

void CTextTool::transferFocusBack()
{
    //另外需要将焦点转移到text
    auto pView = CManageViewSigleton::GetInstance()->getCurView();
    pView->captureFocus();
}

void CTextTool::onSizeChanged(int fontSz, bool backFocus)
{
    fontSz = qMin(qMax(fontSz, 8), 500);
    QSignalBlocker blokerLineEditor(m_fontSize->lineEdit());
    m_fontSize->lineEdit()->setText(QString("%1px").arg(fontSz));

    QSignalBlocker bloker(m_fontSize);
    if (_currenFontSize != fontSz) {
        _currenFontSize = fontSz;
        CCmdBlock block(isTextEnableUndoThisTime() ? drawApp->currentDrawScence()->selectGroup() : nullptr);
        emit drawApp->attributionsWgt()->attributionChanged(EFontSize, fontSz, EChanged, false);
        if (backFocus)
            transferFocusBack();
    }
}
bool CTextTool::isTextEnableUndoThisTime()
{
    //如果当前是激活的状态(意味着文字是处于编辑状态),那么文字外部撤销还原栈不用进行数据收集
    auto curView = CManageViewSigleton::GetInstance()->getCurView();
    auto curActiveBzItem = curView->activeProxDrawItem();
    if (curActiveBzItem != nullptr) {
        if (curActiveBzItem->type() == TextType) {
            return false;
        }
    }
    return CUndoRedoCommand::isRecordEmpty();
}

void CTextTool::toolCreatItemFinish(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    if (pInfo != nullptr) {
        CGraphicsTextItem *pItem = dynamic_cast<CGraphicsTextItem *>(pInfo->businessItem);
        if (nullptr != pItem) {
            if (pItem->scene() == nullptr) {
                pItem->drawScene()->addCItem(pItem);
            }

            if (pItem->scene() != nullptr && !rasterItemToLayer(event, pInfo))
                CCmdBlock block(event->scene(), CSceneUndoRedoCommand::EItemAdded, pItem);

            pItem->setTextState(CGraphicsTextItem::EInEdit, true);
            pItem->textEditor()->document()->clearUndoRedoStacks();
            event->scene()->selectItem(pItem);
            drawApp->attributionsWgt()->setFocus();
        }
    }

    IDrawTool::toolCreatItemFinish(event, pInfo);
}

CGraphicsItem *CTextTool::creatItem(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    Q_UNUSED(pInfo)
    if ((event->eventType() == CDrawToolEvent::EMouseEvent && event->mouseButtons() == Qt::LeftButton)
            || event->eventType() == CDrawToolEvent::ETouchEvent) {
        CGraphicsTextItem *pItem =  new CGraphicsTextItem(QObject::tr("Input text here"));
        //初始化不要响应信号
        QSignalBlocker blocker(pItem->textEditor());

        pItem->setPos(event->pos().x(), event->pos().y());
        pItem->textEditor()->setAlignment(Qt::AlignLeft);
        pItem->textEditor()->selectAll();

        CGraphicsView *pView = event->scene()->drawView();

        QFontMetrics fm(pView->getDrawParam()->getTextFont());
        QSizeF size = pItem->textEditor()->document()->size();

        // 设置默认的高度会显示不全,需要设置为字体高度的1.4倍
        pItem->setRect(QRectF(m_sPointPress.x(), m_sPointPress.y(), size.width(), fm.height() * 1.4));

        // 设置新建图元属性
//        pItem->setFontSize(pView->getDrawParam()->getTextFont().pointSize());
//        pItem->setFontFamily(pView->getDrawParam()->getTextFont().family());
//        pItem->setFontStyle(pView->getDrawParam()->getTextFontStyle());
//        pItem->setTextColor(pView->getDrawParam()->getTextColor());

        pItem->textEditor()->document()->clearUndoRedoStacks();
        event->scene()->addCItem(pItem);

        return pItem;
    }
    return nullptr;
}

int CTextTool::minMoveUpdateDistance()
{
    return 0;
}

bool CTextTool::isPressEventHandledByQt(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo)
{
    Q_UNUSED(event)
    Q_UNUSED(pInfo)

    return IDrawTool::isPressEventHandledByQt(event, pInfo);
}

bool CTextTool::rasterItemToLayer(CDrawToolEvent *event, ITERecordInfo *pInfo)
{
    return false;
}

bool CTextTool::eventFilter(QObject *o, QEvent *event)
{
    if (o == m_fontSize->view() || o == m_fontComBox->view() || o == m_fontHeavy->view()) {
        if (event->type() == QEvent::Show) {
            //启动预览
            if (o == m_fontComBox->view()) {
                _activePackup = false;
                _fontViewShowOut = true;
                _cachedFontWeightStyle = m_fontHeavy->currentText();
                //qDebug() << "_cachedFontWeightStyle = " << m_fontComBox->currentText() << "isTextEnableUndoThisTime() = " << isTextEnableUndoThisTime();
                CCmdBlock block(isTextEnableUndoThisTime() ? drawApp->currentDrawScence()->selectGroup() : nullptr, EChangedBegin);
            }

        } else if (event->type() == QEvent::Hide) {
            //启动预览
            if (o == m_fontComBox->view()) {
                //QHideEvent* hideEvent = static_cast<QHideEvent*>(event);
                _fontViewShowOut = false;
                QMetaObject::invokeMethod(this, [ = ]() {
                    //qDebug() << "_activePackup ======== " << _activePackup;
                    if (!_activePackup) {
                        //还原
                        QSignalBlocker blocker(m_fontHeavy);
                        reInitFontWeightComboxItems(m_fontComBox->currentText(), m_fontHeavy);
                        m_fontHeavy->setCurrentText(_cachedFontWeightStyle);
                        emit drawApp->attributionsWgt()->attributionChanged(EFontFamily, m_fontComBox->currentText(), EChanged, false);
                        emit drawApp->attributionsWgt()->attributionChanged(EFontWeightStyle, _cachedFontWeightStyle, EChanged, false);
                        CUndoRedoCommand::clearCommand();
                    } else {
                        emit drawApp->attributionsWgt()->attributionChanged(EFontFamily, m_fontComBox->currentText(), EChangedFinished, false);
                        reInitFontWeightComboxItems(m_fontComBox->currentText(), m_fontHeavy);
                        CCmdBlock block(drawApp->currentDrawScence()->selectGroup(), EChangedFinished);
                    }
                }, Qt::QueuedConnection);
            }
            transferFocusBack();

            return true;
        }
    } else if (o == m_fontSize) {
        //实现按键响应的字体大小设置
        if (event->type() == QEvent::KeyPress) {
            QKeyEvent *keyEvent = static_cast<QKeyEvent *>(event);
            if (Qt::Key_Up == keyEvent->key() || Qt::Key_PageUp == keyEvent->key()) {
                int size = m_fontSize->currentText().replace("px", "").toInt();
                ++size;
                size = qMin(qMax(size, 8), 500);
                m_fontSize->setCurrentText(QString("%1px").arg(size));
                onSizeChanged(size, false);
                return true;
            }

            if (Qt::Key_Down == keyEvent->key() || Qt::Key_PageDown == keyEvent->key()) {
                int size = m_fontSize->currentText().replace("px", "").toInt();
                --size;
                size = qMin(qMax(size, 8), 500);
                m_fontSize->setCurrentText(QString("%1px").arg(size));
                onSizeChanged(size, false);
                return true;
            }
        }
    }

    return IDrawTool::eventFilter(o, event);
}

void CTextTool::initFontFamilyWidget(DComboBox *fontHeavy)
{
    auto attriMangerWgt = drawApp->attributionsWgt();

    //文字字体名设置控件
    auto fontFamily = new CComBoxSettingWgt(tr("Font"));
    fontFamily->setAttribution(EFontFamily);
    auto fontComboBox = new QFontComboBox;
    drawApp->setWidgetAccesibleName(fontComboBox, "Text font family comboBox");
    fontComboBox->setFocusPolicy(Qt::NoFocus);
    fontComboBox->setEditable(true);
    fontComboBox->lineEdit()->setReadOnly(true);
    fontFamily->setComboBox(fontComboBox);

    // 设置默认的字体类型为思源宋黑体，没有该字体则选择系统第一个默认字体
    QFontDatabase fontbase;
    QString sourceHumFont = QObject::tr("Source Han Sans CN");
    if (!fontbase.families().contains(sourceHumFont)) {
        sourceHumFont = fontbase.families().first();
        //CAttributeManagerWgt::installComAttributeWgt(EFontFamily, fontFamily, sourceHumFont);
    }/* else {
        CAttributeManagerWgt::installComAttributeWgt(EFontFamily, fontFamily, QStringLiteral("思源黑体 CN"));
    }*/
    CAttributeManagerWgt::installComAttributeWgt(EFontFamily, fontFamily, sourceHumFont);

    connect(fontComboBox, QOverload<const QString &>::of(&QComboBox::activated), this, [ = ](const QString & family) {
        Q_UNUSED(family)
        _activePackup = true;
    });

    connect(fontComboBox, QOverload<const QString &>::of(&QComboBox::highlighted), this, [ = ](const QString & family) {
        //预览的不用支持撤销还原
        if (_fontViewShowOut) {
            qDebug() << "QComboBox::highlighted ========== " << family << "fontComboBox view = " << fontComboBox->isVisible();
            emit attriMangerWgt->attributionChanged(EFontFamily, family, EChanged, false);
            reInitFontWeightComboxItems(family, fontHeavy);
        }

    });

//    connect(fontComboBox, &QComboBox::currentTextChanged, fontFamily, [ = ](const QString & family) {
//        CCmdBlock block(isTextEnableUndoThisTime() ? drawApp->currentDrawScence()->selectGroup() : nullptr);
//        emit attriMangerWgt->attributionChanged(EFontFamily, family, EChanged, false);
//        reInitFontWeightComboxItems(family, fontHeavy);
//    });

    connect(attriMangerWgt, &CAttributeManagerWgt::updateWgt, fontFamily, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == fontFamily) {
            QSignalBlocker blocker(fontComboBox);
            QString string = var.isValid() ? var.toString() : QString("— —");
            if (string.isEmpty()) {
                string = QString("— —");
            }
            qDebug() << "var = " << var << "string = " << string;
            fontComboBox->setCurrentText(string);
        }
    });
    m_fontComBox = fontComboBox;
    m_fontComBox->view()->installEventFilter(this);

    QSignalBlocker blocker(fontHeavy);
    reInitFontWeightComboxItems(sourceHumFont, fontHeavy);
}

void CTextTool::initFontWeightWidget()
{
    static QStringList supWeightStyleList;
    if (supWeightStyleList.isEmpty()) {
        supWeightStyleList << "Regular" << "Black" << "DemiBold" << "Bold" << "Medium" << "Light" << "ExtraLight";
    }
    auto attriMangerWgt = drawApp->attributionsWgt();
    //文字字重设置控件
    auto fontWeightStyle = new CComBoxSettingWgt;
    fontWeightStyle->setAttribution(EFontWeightStyle);
    auto ftStyleComboBox = new QComboBox;
    drawApp->setWidgetAccesibleName(ftStyleComboBox, "Text font style comboBox");
    ftStyleComboBox->setFocusPolicy(Qt::NoFocus);
    ftStyleComboBox->setMinimumWidth(130);
    ftStyleComboBox->setEditable(true);
    ftStyleComboBox->lineEdit()->setReadOnly(true);
    ftStyleComboBox->addItems(supWeightStyleList);
    fontWeightStyle->setComboBox(ftStyleComboBox);

    connect(ftStyleComboBox, &QComboBox::currentTextChanged, fontWeightStyle, [ = ](const QString & style) {
        //qDebug() << "set sytle to ========= " << style;
        CCmdBlock block(isTextEnableUndoThisTime() ? drawApp->currentDrawScence()->selectGroup() : nullptr);
        emit attriMangerWgt->attributionChanged(EFontWeightStyle, style, EChanged, false);

    });
    connect(attriMangerWgt, &CAttributeManagerWgt::updateWgt, fontWeightStyle, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == fontWeightStyle) {
            QSignalBlocker blocker(ftStyleComboBox);
            QString string = var.isValid() ? var.toString() : QString("— —");
            if (string.isEmpty()) {
                string = QString("— —");
            }
            ftStyleComboBox->setCurrentText(string);
        }
    });
    CAttributeManagerWgt::installComAttributeWgt(EFontWeightStyle, fontWeightStyle, supWeightStyleList.first());

    m_fontHeavy = ftStyleComboBox;

    m_fontHeavy->view()->installEventFilter(this);
}

void CTextTool::reInitFontWeightComboxItems(const QString &family, DComboBox *fontHeavy)
{
    static QStringList supWeightStyleList;
    if (supWeightStyleList.isEmpty()) {
        supWeightStyleList << "Regular" << "Black" << "DemiBold" << "Bold" << "Medium" << "Light" << "ExtraLight";
    }
    auto currentWeightStyle = fontHeavy->currentText();
    int canKeepIndex = -1;
    {
        QSignalBlocker blocker(fontHeavy);
        fontHeavy->clear();
        QFontDatabase base;
        QStringList listStylyName = base.styles(family);
        for (int i = 0; i < supWeightStyleList.count(); ++i) {
            auto style = supWeightStyleList.at(i);
            if (listStylyName.contains(style)) {
                fontHeavy->addItem(style);
            }
        }
        canKeepIndex = fontHeavy->findText(currentWeightStyle);
        fontHeavy->setCurrentIndex(-1);
    }
    //qDebug() << "currentWeightStyle = " << currentWeightStyle << "canKeepIndex = " << canKeepIndex;
    if (canKeepIndex < 0) {
        fontHeavy->setCurrentIndex(0);
    } else {
        fontHeavy->setCurrentIndex(canKeepIndex);
    }
}

void CTextTool::initFontFontSizeWidget()
{
    _defaultFontSizeSet << 8 << 10 << 12 << 14 << 16 << 18 << 24 << 36 << 48 << 60 << 72 << 100;
    auto attriMangerWgt = drawApp->attributionsWgt();

    //文字字体大小设置控件
    auto fontSize = new CComBoxSettingWgt(tr("Size"));
    fontSize->setAttribution(EFontSize);
    auto ftSizeComboBox = new QComboBox;
    ftSizeComboBox->setEditable(true);
    ftSizeComboBox->setMinimumWidth(105);
    fontSize->setComboBox(ftSizeComboBox);
    drawApp->setWidgetAccesibleName(fontSize->comboBox(), "Text font size comboBox");
    for (int i = 0; i < _defaultFontSizeSet.count(); ++i) {
        auto text = QString("%1px").arg(_defaultFontSizeSet[i]);
        ftSizeComboBox->addItem(text);
    }
    QRegExp regx("[0-9]*p?x?");
    QValidator *validator = new QRegExpValidator(regx, ftSizeComboBox);
    ftSizeComboBox->setValidator(validator);
    connect(ftSizeComboBox, QOverload<const QString &>::of(&QComboBox::currentIndexChanged), fontSize, [ = ](const QString & fontSize) {
        int size = QString(fontSize).remove("px").toInt();
        onSizeChanged(size);
    });
    connect(ftSizeComboBox->lineEdit(), &QLineEdit::editingFinished, fontSize, [ = ]() {
        int size = QString(ftSizeComboBox->currentText()).remove("px").toInt();
        onSizeChanged(size);
    });

    connect(attriMangerWgt, &CAttributeManagerWgt::updateWgt, fontSize, [ = ](QWidget * pWgt, const QVariant & var) {
        if (pWgt == fontSize) {
            QSignalBlocker blocker(ftSizeComboBox);
            QString text = (!var.isValid() || var.toInt() == 0) ?  QStringLiteral("— —") : QString("%1px").arg(var.toInt());
            ftSizeComboBox->setCurrentText(text);
        }
    });
    CAttributeManagerWgt::installComAttributeWgt(EFontSize, fontSize, _currenFontSize);

    ftSizeComboBox->lineEdit()->setReadOnly(/*true*/Application::isTabletSystemEnvir());

    m_fontSize = ftSizeComboBox;

    //combox的lineedit控件编辑文字后不用添加新的item
    m_fontSize->setInsertPolicy(QComboBox::NoInsert);

    m_fontSize->view()->installEventFilter(this);
    m_fontSize->installEventFilter(this);
}
