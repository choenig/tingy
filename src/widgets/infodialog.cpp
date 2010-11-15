/****************************************************************************
** Author: Christian Hoenig <tingy@mail.hoenig.cc>
** Copyright (c) 2010 Christian Hoenig
**
** This file is part of tingy.
**
** tingy is free software: you can redistribute it and/or modify
** it under the terms of the GNU General Public License as published by
** the Free Software Foundation, either version 3 of the License, or
** (at your option) any later version.
**
** tingy is distributed in the hope that it will be useful,
** but WITHOUT ANY WARRANTY; without even the implied warranty of
** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
** GNU General Public License for more details.
**
** You should have received a copy (license.txt) of the
** GNU General Public License along with tingy.
** If not, see <http://www.gnu.org/licenses/>.
****************************************************************************/

#include "infodialog.h"

#include "ui_infodialog.h"

InfoDialog::InfoDialog(QWidget *parent)
    : TingyDialog(parent, "information"), ui(new Ui::InfoDialog)
{
    ui->setupUi(centralWidget());
    resize(500,200);
}

InfoDialog::~InfoDialog()
{
    delete ui;
}
