/**
 * Copyright (C) 2015 Deepin Technology Co., Ltd.
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 **/

#include "inputview.h"

#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>
#include <QTimer>
#include <QRegExp>
#include <QRegExpValidator>
#include <QDebug>
#include <QBitmap>
#include <QClipboard>
#include <QApplication>

#include "widgets/tiplabel.h"

#include "constants.h"
#include "../helper.h"

#define ACCEL_KEY(k) QString()

DWIDGET_USE_NAMESPACE

static const QString TokenLineEditStyle = "QLineEdit#TokenLineEdit { "
        "background: white; "
        "border: 1px solid rgba(0, 0, 0, 0.1); "
        "border-radius: 4px; "
        "}";

InputView::InputView(QWidget *p)
    : AbstractView(p),
      m_validator(new QRegExpValidator(*new QRegExp("[A-Za-z0-9]{6}"), this))
{
    setObjectName("InputView");
    m_buttonFlag = InputView::btncancel;

    m_connectButton = new DBaseButton(tr("Cancel"));
    m_connectButton->setFixedSize(160, 36);

    QObject::connect(m_connectButton, &DBaseButton::clicked, [this] {
        switch (m_buttonFlag)
        {
        case InputView::btncancel:
            emit cancel();
            break;
        case InputView::btnconnect:
            emitConnect();
            break;
        }
    });

    initialize();

    focus();
}

void InputView::emitConnect()
{
    emit connect(m_tokenEdit->text().trimmed());
}

void InputView::connectToClient()
{
    if (InputView::btnconnect == m_buttonFlag) {
        emitConnect();
    }
}

QWidget *InputView::createMainWidget()
{
    auto mainWidget = new QWidget;
    auto layout = new QVBoxLayout;
    layout->setSpacing(0);
    layout->setMargin(0);

    m_tokenEdit = new CustomisedLineEdit("");
    m_tokenEdit->setMaxLength(6);

    m_tokenEdit->setAlignment(Qt::AlignCenter);
    m_tokenEdit->setFixedWidth(DRA::ModuleContentWidth);
    m_tokenEdit->setFixedHeight(70);

    // Set style for our token edit box.
    m_tokenEdit->setObjectName("TokenLineEdit");
    m_tokenEdit->setStyleSheet(TokenLineEditStyle);

    QFont font = m_tokenEdit->font();
    font.setPixelSize(30);
    font.setLetterSpacing(QFont::AbsoluteSpacing, 16);
    m_tokenEdit->setFont(font);

    QObject::connect(m_tokenEdit, SIGNAL(returnPressed()), this, SLOT(connectToClient()));
    QObject::connect(m_tokenEdit, &QLineEdit::textChanged, [this](const QString & token) {
        qDebug() << "valid token";
        QString copyToken = token;
        int pos = 0;

        m_connectButton->setText(tr("Cancel"));
        m_buttonFlag = InputView::btncancel;

        QRegExpValidator tipsValidator(*new QRegExp("[A-Za-z0-9]{0,6}"));
        if (tipsValidator.validate(copyToken, pos) != QValidator::Acceptable) {
            setTips(tr("Invalid verification code, please retype!"));
        } else {
            setTips(tr("Input verification code and \"Connect\" to start remote access"));
        }

        if (m_validator->validate(copyToken, pos) == QValidator::Acceptable) {
            m_connectButton->setText(tr("Connect"));
            m_buttonFlag = InputView::btnconnect;
        }
    });

    addButton(m_connectButton);

    layout->addSpacing(56);
    layout->addWidget(m_tokenEdit, 0, Qt::AlignCenter);

    m_tip = new TipLabel(this);
    m_tip->setText(tr("Input verification code and \"Connect\" to start remote access"));
    m_tip->setFixedSize(DRA::TipLabelMaxWidth, DRA::TipLabelMaxHeight);

    layout->addSpacing(20);
    layout->addWidget(m_tip, 0, Qt::AlignCenter);
//    layout->addS(30);
//    layout->addLayout(m_buttonHLayout);
    layout->addStretch();
    mainWidget->setLayout(layout);
    return mainWidget;
}

void InputView::focus()
{
    qDebug() << "focus token input widget";
    m_tokenEdit->setFocus();
}

void InputView::setTips(const QString &tips)
{
    m_tip->setText(tips);
}

void CustomisedLineEdit::contextMenuEvent(QContextMenuEvent *event)
{
    QMenu *menu = new QMenu(this);
    menu->setObjectName(QLatin1String("qt_edit_menu"));
    QAction *action = 0;

    if (!isReadOnly()) {
        action = menu->addAction(QLineEdit::tr("Cu&t") + ACCEL_KEY(QKeySequence::Cut));
        action->setEnabled(!this->isReadOnly() && this->hasSelectedText()
                           && this->echoMode() == QLineEdit::Normal);
        connect(action, SIGNAL(triggered()), SLOT(cut()));


        action = menu->addAction(QLineEdit::tr("&Copy") + ACCEL_KEY(QKeySequence::Copy));
        action->setEnabled(this->hasSelectedText()
                           && this->echoMode() == QLineEdit::Normal);
        connect(action, SIGNAL(triggered()), SLOT(copy()));
    }

    if (!isReadOnly()) {
        action = menu->addAction(QLineEdit::tr("&Paste") + ACCEL_KEY(QKeySequence::Paste));
        action->setEnabled(!this->isReadOnly() && !QApplication::clipboard()->text().isEmpty());
        connect(action, SIGNAL(triggered()), SLOT(paste()));
    }

    if (!isReadOnly()) {
        action = menu->addAction(QLineEdit::tr("Delete"));
        action->setEnabled(!this->isReadOnly() && !this->text().isEmpty() && this->hasSelectedText());
        connect(action, SIGNAL(triggered()), this, SLOT(clear()));
    }

    menu->exec(event->globalPos());
    menu->deleteLater();
    event->accept();
}
