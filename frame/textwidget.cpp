/*
 * Copyright (C) 2019 ~ %YEAR% Deepin Technology Co., Ltd.
 *
 * Author:     Renran
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
#include "textwidget.h"

#include <DLabel>
#include "widgets/textcolorbutton.h"
#include "widgets/seperatorline.h"
#include "widgets/dmenucombobox.h"

#include "utils/cvalidator.h"

#include "drawshape/cdrawparamsigleton.h"

#include "frame/cviewmanagement.h"
#include "frame/cgraphicsview.h"

#include "service/cmanagerattributeservice.h"

#include <DLabel>
#include <QHBoxLayout>
#include <QFont>
#include <QLineEdit>

const int BTN_SPACING = 6;
const int SEPARATE_SPACING = 5;
const int TEXT_SIZE = 12;
TextWidget::TextWidget(DWidget *parent)
    : DWidget(parent)
    , m_bSelect(false)
{
    initUI();
    initConnection();
}

TextWidget::~TextWidget()
{

}

void TextWidget::initUI()
{
    m_fillBtn = new TextColorButton( this);
    m_fillBtn->setFocusPolicy(Qt::NoFocus);

    QFont ft;
    ft.setPixelSize(TEXT_SIZE);

    m_textSeperatorLine = new SeperatorLine(this);

    m_fontFamilyLabel = new DLabel(this);
    m_fontFamilyLabel->setText(tr("Font"));
    m_fontFamilyLabel->setFont(ft);
    m_fontComBox = new CFontComboBox(this);
    m_fontComBox->setFontFilters(DFontComboBox::AllFonts);

    m_fontComBox->setFixedSize(QSize(240, 36));
    m_fontComBox->setCurrentIndex(0);
    m_fontComBox->setEditable(true);
    m_fontComBox->lineEdit()->setReadOnly(true);
    m_fontComBox->lineEdit()->setFont(ft);
    QString strFont = m_fontComBox->currentText();
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextFont(strFont);

    m_fontHeavy = new DMenuComboBox(this); // 字体类型
    m_fontHeavy->setFixedSize(QSize(100, 36));
    m_fontHeavy->setMenuMaxWidth(100);
    m_fontHeavy->addItem(tr("Regular"));

    m_fontsizeLabel = new DLabel(this);
    m_fontsizeLabel->setText(tr("Size")); // 字号
    m_fontsizeLabel->setFixedSize(QSize(28, 20));
    m_fontsizeLabel->setFont(ft);
    m_fontSize = new DSpinBox(this);
    m_fontSize->setFixedSize(QSize(100, 36));
    m_fontSize->setRange(8, 500);
    m_fontSize->setSuffix("px");

    QHBoxLayout *layout = new QHBoxLayout(this);
    layout->setMargin(0);
    layout->setSpacing(BTN_SPACING);
    layout->addStretch();
    layout->addWidget(m_fillBtn);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_textSeperatorLine);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_fontFamilyLabel);
    layout->addWidget(m_fontComBox);
    layout->addWidget(m_fontHeavy);
    layout->addSpacing(SEPARATE_SPACING);
    layout->addWidget(m_fontsizeLabel);
    layout->addWidget(m_fontSize);
    layout->addStretch();
    setLayout(layout);
}

void TextWidget::updateTheme()
{
    m_textSeperatorLine->updateTheme();
}

void TextWidget::slotFontSizeValueChanged(int size)
{
    CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextSize(size);
    emit signalTextFontSizeChanged();
    //隐藏调色板
    showColorPanel(DrawStatus::TextFill, QPoint(), false);
    CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(TextSize, size);
}

void TextWidget::updateMultCommonShapWidget(QMap<EDrawProperty, QVariant> propertys)
{
    m_fillBtn->setVisible(false);
    m_textSeperatorLine->setVisible(false);
    m_fontFamilyLabel->setVisible(false);
    m_fontComBox->setVisible(false);
    m_fontHeavy->setVisible(false);
    m_fontsizeLabel->setVisible(false);
    m_fontSize->setVisible(false);
    for (int i = 0; i < propertys.size(); i++) {
        EDrawProperty property = propertys.keys().at(i);
        switch (property) {
        case TextColor:
            m_fillBtn->setVisible(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_fillBtn->setIsMultColorSame(false);
            } else {
                m_fillBtn->setColor(propertys[property].value<QColor>());
            }
            m_fillBtn->update();
            break;
        case TextFont:
            m_fontFamilyLabel->setVisible(true);
            m_fontComBox->setVisible(true);
            m_fontHeavy->setVisible(true);
            m_fontHeavy->blockSignals(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_fontComBox->setCurrentText("--");
                m_fontHeavy->cleanAllMenuItem();
                m_fontHeavy->addItem(tr("--"));
                m_fontHeavy->setCurrentText("--");
            } else {
                QFont font = propertys[property].value<QFont>();
                m_fontComBox->setCurrentText(font.family());
                QFontDatabase base;
                QStringList listStylyName = base.styles(font.family());
                listStylyName.removeOne("Regular");
                m_fontHeavy->blockSignals(true);
                m_fontHeavy->cleanAllMenuItem();
                m_fontHeavy->addItem(tr("Regular"));
                for (QString style : listStylyName) {
                    m_fontHeavy->addItem(style);
                }
                m_fontHeavy->setCurrentText(font.styleName());
                m_fontHeavy->blockSignals(false);
            }
            m_fontComBox->update();
            m_fontHeavy->blockSignals(false);
            break;
        case TextHeavy:
            m_fontHeavy->setVisible(true);
            m_fontHeavy->blockSignals(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_fontHeavy->cleanAllMenuItem();
                m_fontHeavy->addItem(tr("--"));
                m_fontHeavy->setCurrentText("--");
            } else {
                QFont font = propertys[TextFont].value<QFont>();
                QFontDatabase base;
                QStringList listStylyName = base.styles(font.family());
                listStylyName.removeOne("Regular");
                m_fontHeavy->blockSignals(true);
                m_fontHeavy->cleanAllMenuItem();
                m_fontHeavy->addItem(tr("Regular"));
                for (QString style : listStylyName) {
                    m_fontHeavy->addItem(style);
                }
                m_fontHeavy->setCurrentText(font.styleName());
                m_fontHeavy->blockSignals(false);
            }
            m_fontHeavy->update();
            m_fontHeavy->blockSignals(false);
            break;
        case TextSize:
            m_fontsizeLabel->setVisible(true);
            m_fontSize->setVisible(true);
            if (propertys[property].type() == QVariant::Invalid) {
                m_fontHeavy->blockSignals(true);
                m_fontSize->setValue(-1);
                m_fontHeavy->blockSignals(false);
            } else {
                m_fontSize->setValue(propertys[property].toInt());
            }
            break;
        default:
            break;
        }
    }
}

void TextWidget::initConnection()
{
    connect(m_fillBtn, &TextColorButton::btnCheckStateChanged, this, [ = ](bool show) {

        QPoint btnPos = mapToGlobal(m_fillBtn->pos());
        QPoint pos(btnPos.x() + m_fillBtn->width() / 2,
                   btnPos.y() + m_fillBtn->height());

        showColorPanel(DrawStatus::TextFill, pos, show);
    });

    connect(this, &TextWidget::resetColorBtns, this, [ = ] {
        m_fillBtn->resetChecked();
    });

    connect(m_fontComBox, QOverload<const QString &>::of(&DFontComboBox::activated), this, [ = ](const QString & str) {
        m_bSelect = false;
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextFont(str);
        emit signalTextFontFamilyChanged();
    });
    connect(m_fontComBox, QOverload<const QString &>::of(&DFontComboBox::highlighted), this, [ = ](const QString & str) {
        m_bSelect = true;
        m_oriFamily = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont().family();
        CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextFont(str);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::TextFont,
                                                                             CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont());
        emit signalTextFontFamilyChanged();
    });
    connect(m_fontComBox, &CFontComboBox::signalhidepopup, this, [ = ]() {
        if (m_bSelect) {
            CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->setTextFont(m_oriFamily);
            emit signalTextFontFamilyChanged();
        }
        m_bSelect = false;
    });
    connect(m_fontComBox,  QOverload<const QString &>::of(&CFontComboBox::currentIndexChanged), this, [ = ](const QString & family) {
        QFontDatabase base; //("Medium", "Bold", "ExtraLight", "Regular", "Heavy", "Light", "SemiBold")
        QStringList listStylyName = base.styles(family);
        listStylyName.removeOne("Regular");
        m_fontHeavy->blockSignals(true);
        m_fontHeavy->cleanAllMenuItem();
        m_fontHeavy->addItem(tr("Regular"));
        for (QString style : listStylyName) {
            m_fontHeavy->addItem(style);
        }
        m_fontHeavy->blockSignals(false);
    });

    // 字体大小
    connect(m_fontSize, SIGNAL(valueChanged(int)), this, SLOT(slotFontSizeValueChanged(int)));
    m_fontSize->setValue(14);

    // 字体重量
    connect(m_fontHeavy, &DMenuComboBox::signalCurrentTextChanged, this, [ = ](const QString & str) {
        // ("Medium", "Bold", "ExtraLight", "Regular", "Heavy", "Light", "SemiBold")
        QString style = "Regular";
        if (str == tr("Medium")) {
            style = "Medium";
        } else if (str == tr("Bold")) {
            style = "Bold";
        } else if (str == tr("ExtraLight")) {
            style = "ExtraLight";
        } else if (str == tr("Heavy")) {
            style = "Heavy";
        } else if (str == tr("Light")) {
            style = "Light";
        } else if (str == tr("Thin")) {
            style = "Thin";
        } else if (str == tr("SemiBold")) {
            style = "SemiBold";
        }
        CManagerAttributeService::getInstance()->setTextFamilyStyle(
            static_cast<CDrawScene *>(CManageViewSigleton::GetInstance()->getCurView()->scene()), style);
        CManagerAttributeService::getInstance()->setItemsCommonPropertyValue(EDrawProperty::TextHeavy, style);

        //隐藏调色板
        showColorPanel(DrawStatus::TextFill, QPoint(), false);
    });
}

void TextWidget::updateTextWidget()
{
    m_fillBtn->updateConfigColor();
    QFont font = CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFont();

    if (CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getSingleFontFlag()) {
        m_fontComBox->setCurrentText(font.family());
        QFontDatabase base; //("Medium", "Bold", "ExtraLight", "Regular", "Heavy", "Light", "SemiBold")
        QStringList listStylyName = base.styles(font.family());
        listStylyName.removeOne("Regular");
        m_fontHeavy->blockSignals(true);
        m_fontHeavy->cleanAllMenuItem();
        m_fontHeavy->addItem(tr("Regular"));
        for (QString style : listStylyName) {
            m_fontHeavy->addItem(style);
        }
        m_fontHeavy->setCurrentText(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextFontStyle());
        m_fontHeavy->blockSignals(false);
    } else {
        m_fontComBox->setCurrentIndex(-1);
        m_fontComBox->setCurrentText("--");
        m_fontHeavy->setCurrentText("--");
    }

    int fontSize = int(CManageViewSigleton::GetInstance()->getCurView()->getDrawParam()->getTextSize());
    if (fontSize != m_fontSize->value()) {
        m_fontSize->setValue(fontSize);
    }

    m_fillBtn->setVisible(true);
    m_textSeperatorLine->setVisible(true);
    m_fontFamilyLabel->setVisible(true);
    m_fontComBox->setVisible(true);
    m_fontHeavy->setVisible(true);
    m_fontsizeLabel->setVisible(true);
    m_fontSize->setVisible(true);
    m_fillBtn->setIsMultColorSame(true);
    //CManagerAttributeService::getInstance()->refreshSelectedCommonProperty();
}

void TextWidget::updateTextColor()
{
    m_fillBtn->updateConfigColor();
//    m_fillBtn->clearFocus();
//    if (qApp->focusWidget() != nullptr) {
//        qApp->focusWidget()->hide();
//    }
}
