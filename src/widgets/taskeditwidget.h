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
#pragma once

#include <core/task.h>
#include <widgets/tingydialog.h>

namespace Ui { class TaskEditWidget; }

class TaskEditWidget : public TingyDialog
{
    Q_OBJECT

public:
    TaskEditWidget(QWidget * parent = 0);
    ~TaskEditWidget();

    Task exec(const Task & task);

private:
    Ui::TaskEditWidget *ui;
};
