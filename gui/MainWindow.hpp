#ifndef MAINWINDOW_HPP
#define MAINWINDOW_HPP

#include <QMainWindow>

#include "../Restaurant.hpp"

class QStackedWidget;
class QWidget;
class QLabel;

class MainWindow : public QMainWindow
{
public:
    explicit MainWindow(
        Restaurant& restaurant,
        QWidget* parent = nullptr
    );

private:


    QLabel* clock_label;
    Restaurant& restaurant;
    QStackedWidget* content;

    QWidget* createZonesPage();
    QWidget* createTablesPage(Zone& zone);
    QWidget* createTablePage(Zone& zone, Table& table);
    QWidget* createAddItemPage(Zone& zone, Table& table);
    QWidget* createPaymentPage(Zone& zone, Table& table);

    void showZonesPage();
    void showTablesPage(Zone& zone);
    void showTablePage(Zone& zone, Table& table);

    void showAddItemPage(Zone& zone, Table& table);
    void showPaymentPage(Zone& zone, Table& table);
};

#endif