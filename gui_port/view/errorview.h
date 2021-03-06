/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#ifndef ERRORVIEW_H
#define ERRORVIEW_H

#include "abstractview.h"

class NotifyLabel;
class TipLabel;

class ErrorView : public AbstractView
{
    Q_OBJECT
public:
    ErrorView(QWidget *p = nullptr);
    ErrorView *setText(const QString &text);
    ErrorView *setTips(const QString &tips);

private slots:
    // void onCancelButtonClicked();
    // void onRetryButtonClicked();

private:
    NotifyLabel *m_text;
    TipLabel    	*m_tip;
    QWidget 	*createMainWidget() Q_DECL_OVERRIDE;
};

#endif // ERRORVIEW_H
