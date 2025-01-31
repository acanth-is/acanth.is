// SPDX-FileCopyrightText: 2021 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "ui_licenseagreement.h"

#include <QDialog>

namespace Ui {
    class LicenseAgreement;
}

class LicenseAgreement : public QDialog {
    Q_OBJECT

  public:
    explicit LicenseAgreement(QWidget *parent = 0);
    ~LicenseAgreement();

  private:
    Ui::LicenseAgreement *m_ui;
};
