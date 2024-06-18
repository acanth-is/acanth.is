// SPDX-FileCopyrightText: 2022 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#pragma once

#include "graphmodel.h"

#include <QObject>

class DocumentManager : public QObject {
    Q_OBJECT

    // Note: All this jumping around with the last document is requried to stop the QML
    // Engine from stealing ownership (and destoying) the GraphModels. A saner
    // solution would for example be to return a new or openeed GraphModel directly
    // from the creating/opening functions, however doing that transfers the ownership
    // to QML which then destroys the objects. This is stated in the qt.io: "When data
    // is transferred from C++ to QML, the ownership of the data always remains with
    // C++. The exception to this rule is when a QObject is returned from an explicit
    // C++ method call: in this case, the QML engine assumes ownership of the object".
    // For more discussion and the solution see:
    // https://embeddeduse.com/2020/01/19/address-sanitizers-qml-engine-deletes-c-objects-still-in-use/

    Q_PROPERTY(GraphModel *lastDocument READ lastDocument)

    struct NameDocumentComparator {
        NameDocumentComparator(std::string const &s) : _s(s) {}
        bool operator()(std::pair<std::string, std::unique_ptr<GraphModel>> const &p) {
            return (p.first == _s);
        }
        std::string _s;
    };
    std::vector<std::pair<std::string, std::unique_ptr<GraphModel>>> m_openedDocuments;

    unsigned int m_lastDocumentIndex = 0;

  public:
    DocumentManager();
    Q_INVOKABLE void createEmptyDocument();
    Q_INVOKABLE void removeDocument(unsigned int index);
    Q_INVOKABLE void openDocument(QString urlString);
    Q_INVOKABLE bool hasDocument() { return !m_openedDocuments.empty(); }
    Q_INVOKABLE unsigned int lastDocumentIndex() { return m_lastDocumentIndex; }
    Q_INVOKABLE unsigned int numOpenedDocuments() {
        return static_cast<unsigned int>(m_openedDocuments.size());
    }

    GraphModel *lastDocument() { return m_openedDocuments[m_lastDocumentIndex].second.get(); }
};
