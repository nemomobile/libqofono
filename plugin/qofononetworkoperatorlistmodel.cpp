/****************************************************************************
**
** Copyright (C) 2014 Jolla Ltd.
** Contact: slava.monich@jolla.com
**
** GNU Lesser General Public License Usage
** Alternatively, this file may be used under the terms of the GNU Lesser
** General Public License version 2.1 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 2.1 requirements
** will be met: http://www.gnu.org/licenses/old-licenses/lgpl-2.1.html.
**
****************************************************************************/

#include "qofononetworkoperatorlistmodel.h"

QOfonoNetworkOperatorListModel::QOfonoNetworkOperatorListModel(QObject *parent) :
    QAbstractListModel(parent)
{
    netreg = new QOfonoNetworkRegistration(this);
    onNetworkOperatorsChanged(netreg->networkOperators());
    connect(netreg,
        SIGNAL(validChanged(bool)),
        SIGNAL(validChanged(bool)));
    connect(netreg,
        SIGNAL(modemPathChanged(QString)),
        SIGNAL(modemPathChanged(QString)));
    connect(netreg,
        SIGNAL(networkOperatorsChanged(QStringList)),
        SLOT(onNetworkOperatorsChanged(QStringList)));
}

QString QOfonoNetworkOperatorListModel::modemPath() const
{
    return netreg->modemPath();
}

void QOfonoNetworkOperatorListModel::setModemPath(const QString &path)
{
    netreg->setModemPath(path);
}

bool QOfonoNetworkOperatorListModel::isValid() const
{
    return netreg->isValid();
}

int QOfonoNetworkOperatorListModel::rowCount(const QModelIndex &) const
{
    return operators.count();
}

int QOfonoNetworkOperatorListModel::count() const
{
    return operators.count();
}

QVariant QOfonoNetworkOperatorListModel::data(const QModelIndex &index, int role) const
{
    int row = index.row();
    if (row >= 0 && row < operators.count()) {
        QOfonoNetworkOperator* op = netreg->networkOperator(operators[row]);
        if (op) {
            switch (role) {
            case PathRole:   return op->operatorPath();
            case NameRole:   return op->name();
            case StatusRole: return op->status();
            case MccRole:    return op->mcc();
            case MncRole:    return op->mnc();
            case TechRole:   return op->technologies();
            case InfoRole:   return op->additionalInfo();
            }
        }
    }
    qWarning() << index << role;
    return QVariant();
}

QHash<int,QByteArray> QOfonoNetworkOperatorListModel::roleNames() const
{
    QHash<int, QByteArray> roles;
    roles[PathRole]   = "operatorPath";
    roles[NameRole]   = "name";
    roles[StatusRole] = "status";
    roles[MccRole]    = "mcc";
    roles[MncRole]    = "mnc";
    roles[TechRole]   = "technologies";
    roles[InfoRole]   = "additionalInfo";
    return roles;
}

void QOfonoNetworkOperatorListModel::operatorPropertyChanged(OperatorRole role)
{
    QOfonoNetworkOperator* op = (QOfonoNetworkOperator*)sender();
    if (op) {
        int row = operators.indexOf(op->operatorPath());
        if (row >= 0) {
            QModelIndex modelIndex = index(row);
            QVector<int> roles;
            roles.append(role);
            Q_EMIT dataChanged(modelIndex, modelIndex, roles);
        }
    }
}

void QOfonoNetworkOperatorListModel::onNetworkOperatorsChanged(const QStringList &list)
{
    beginResetModel();
    const int oldCount = operators.count();
    const int n = list.count();
    operators = list;
    for (int i=0; i<n; i++) {
        QOfonoNetworkOperator* op = netreg->networkOperator(list[i]);
        if (op) {
            op->disconnect(this);
            connect(op, SIGNAL(operatorPathChanged(QString)), SLOT(onOperatorPathChanged(QString)));
            connect(op, SIGNAL(nameChanged(QString)), SLOT(onOperatorNameChanged(QString)));
            connect(op, SIGNAL(statusChanged(QString)), SLOT(onOperatorStatusChanged(QString)));
            connect(op, SIGNAL(mccChanged(QString)), SLOT(onOperatorMccChanged(QString)));
            connect(op, SIGNAL(mncChanged(QString)), SLOT(onOperatorMncChanged(QString)));
            connect(op, SIGNAL(technologiesChanged(QStringList)), SLOT(onOperatorTechChanged(QStringList)));
            connect(op, SIGNAL(additionalInfoChanged(QString)), SLOT(onOperatorInfoChanged(QString)));
        }
    }
    if (oldCount != n) {
        Q_EMIT countChanged(n);
    }
    endResetModel();
}

void QOfonoNetworkOperatorListModel::onOperatorPathChanged(const QString &)
{
    operatorPropertyChanged(PathRole);
}

void QOfonoNetworkOperatorListModel::onOperatorNameChanged(const QString &)
{
    operatorPropertyChanged(NameRole);
}

void QOfonoNetworkOperatorListModel::onOperatorStatusChanged(const QString &)
{
    operatorPropertyChanged(StatusRole);
}

void QOfonoNetworkOperatorListModel::onOperatorMccChanged(const QString &)
{
    operatorPropertyChanged(MccRole);
}

void QOfonoNetworkOperatorListModel::onOperatorMncChanged(const QString &)
{
    operatorPropertyChanged(MncRole);
}

void QOfonoNetworkOperatorListModel::onOperatorTechChanged(const QStringList &)
{
    operatorPropertyChanged(TechRole);
}

void QOfonoNetworkOperatorListModel::onOperatorInfoChanged(const QString &)
{
    operatorPropertyChanged(InfoRole);
}
