// SPDX-FileCopyrightText: 2022 Petros Koutsolampros
//
// SPDX-License-Identifier: GPL-3.0-or-later

#include "documentmanager.h"

#include <QDir>
#include <QUrl>

DocumentManager::DocumentManager() {}

void DocumentManager::createEmptyDocument() {
    std::string newDocName = "Untitled";
    size_t counter = 1;
    while (std::find_if(m_openedDocuments.begin(), m_openedDocuments.end(),
                        NameDocumentComparator(newDocName)) != m_openedDocuments.end()) {
        newDocName = "Untitled " + std::to_string(counter);
        ++counter;
    }
    m_openedDocuments.push_back(
        std::make_pair(newDocName, std::unique_ptr<GraphModel>(new GraphModel(newDocName))));
    m_lastDocumentIndex = static_cast<unsigned int>(m_openedDocuments.size() - 1);
}

void DocumentManager::removeDocument(unsigned int index) {
    auto openDocument = m_openedDocuments.begin() + index;
    m_openedDocuments.erase(openDocument);
}

void DocumentManager::openDocument(QString urlString) {
    std::string fileName;
    const QUrl url(urlString);
    if (url.isLocalFile()) {
        fileName = QDir::toNativeSeparators(url.toLocalFile()).toStdString();
    } else {
        fileName = urlString.toStdString();
    }
    auto doc = std::find_if(m_openedDocuments.begin(), m_openedDocuments.end(),
                            NameDocumentComparator(fileName));
    if (doc != m_openedDocuments.end()) {
        m_lastDocumentIndex =
            static_cast<unsigned int>(std::distance(m_openedDocuments.begin(), doc));
        return;
    }

    m_openedDocuments.push_back(
        std::make_pair(fileName, std::unique_ptr<GraphModel>(new GraphModel(fileName))));
    m_lastDocumentIndex = static_cast<unsigned int>(m_openedDocuments.size() - 1);
}
