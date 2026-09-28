#include <QApplication>

#include "MainWindow.hpp"

#include "../Restaurant.hpp"
#include "../RestaurantData.hpp"

int main(int argc, char *argv[])
{
    QApplication app(argc, argv);

    Restaurant restaurant("CTRL + EAT");

    initialize_restaurant(restaurant);

    MainWindow window(restaurant);

    window.show();

    return app.exec();
}