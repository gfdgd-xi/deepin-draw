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
#ifndef CTEXTTOOL_H
#define CTEXTTOOL_H
#include "idrawtool.h"

class CTextTool : public IDrawTool
{
    Q_OBJECT
public:
    CTextTool();

    virtual ~CTextTool() override;

    DrawAttribution::SAttrisList attributions() override;

    void registerAttributionWidgets() override;

protected:

    /**
     * @brief toolButton 定义工具的激活按钮
     */
    QAbstractButton* initToolButton() override;

    /**
     * @brief toolFinish 工具执行的结束
     * @param event 事件
     * @param scene 场景
     */
    void toolCreatItemFinish(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief toolFinish 创建图元
     * @param event 事件
     * @param scene 场景
     */
    CGraphicsItem *creatItem(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief minMoveUpdateDistance　工具创造业务图元指针
     */

    int minMoveUpdateDistance() override;

    /**
     * @brief isPressEventHandledByQt　鼠标点下是否要交给qt框架处理
     */
    bool isPressEventHandledByQt(CDrawToolEvent *event, ITERecordInfo *pInfo) override;

    /**
     * @brief autoSupUndoForCreatItem 是否支持在创建生成图元后自动添加到undo栈(默认是true)
     */
    bool autoSupUndoForCreatItem() override {return false;}

    bool rasterItemToLayer(CDrawToolEvent *event, IDrawTool::ITERecordInfo *pInfo) override;

    bool eventFilter(QObject *object, QEvent *e) override;

private:
    void initFontFamilyWidget(DComboBox *fontHeavy);
    void initFontWeightWidget();
    void reInitFontWeightComboxItems(const QString &family, DComboBox *fontHeavy);
    void initFontFontSizeWidget();

    bool isTextEnableUndoThisTime();

    Q_SLOT void transferFocusBack();

    Q_SLOT void onSizeChanged(int fontSz, bool backFocus = true);


private:
    DComboBox *m_fontComBox = nullptr;
    DComboBox *m_fontHeavy = nullptr;
    DComboBox *m_fontSize = nullptr;

    bool      _activePackup = false;
    bool      _fontViewShowOut = false;
    QString   _cachedFontWeightStyle;

    int        _currenFontSize = 14;
    QList<int> _defaultFontSizeSet;

};

#endif // CTEXTTOOL_H
