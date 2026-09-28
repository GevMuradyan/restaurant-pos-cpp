
#include "MainWindow.hpp"
#include "CardTerminal.hpp"

#include <QApplication>
#include <QFrame>
#include <QGridLayout>
#include <QHBoxLayout>
#include <QLabel>
#include <QPainter>
#include <QPainterPath>
#include <QPushButton>
#include <QScrollArea>
#include <QSizePolicy>
#include <QSpacerItem>
#include <QStackedWidget>
#include <QVBoxLayout>
#include <QWidget>
#include <QDateTime>
#include <QTimer>
#include <QTimeZone>
#include <QRandomGenerator>
#include <QStringList>
#include <QPixmap>

#include <algorithm>
#include <memory>

// ============================================================
// CTRL + EAT
// Professional Restaurant POS UI
// ============================================================

namespace
{

// ------------------------------------------------------------
// Color system
// ------------------------------------------------------------

const QString BG             = "#e5e5e8";
const QString SIDEBAR        = "#050505";

const QString CARD           = "#e2f4f2";
const QString CARD_HOVER     = "#c8edc6";
const QString CARD_PRESSED   = "#f7faf9";

const QString BORDER         = "#D7CFC2";
const QString BORDER_HOVER   = "#BFCFC5";

const QString TEXT           = "#060606";
const QString MUTED          = "#7c8580";

const QString GREEN          = "#0d6b1b";
const QString GREEN_LIGHT    = "#E4F1EA";

const QString BLUE           = "#2257ac";
const QString BLUE_LIGHT     = "#EAF0F8";

const QString ORANGE         = "#d68235";
const QString ORANGE_LIGHT   = "#F8EEE4";

const QString RED            = "#d1484a";
const QString RED_LIGHT      = "#F8EAEA";

const QString PURPLE         = "#abd3e1";
const QString PURPLE_LIGHT   = "#F0EBF5";

const QString WHITE          = "#ffffff";


// ------------------------------------------------------------
// General helpers
// ------------------------------------------------------------

QString status_text(TableStatus status)
{
    switch (status)
    {
        case TableStatus::Free:
            return "FREE";

        case TableStatus::Occupied:
            return "OCCUPIED";

        case TableStatus::CallingWaiter:
            return "WAITER";

        case TableStatus::BillRequested:
            return "BILL";
    }

    return "UNKNOWN";
}


QString status_color(TableStatus status)
{
    switch (status)
    {
        case TableStatus::Free:
            return GREEN;

        case TableStatus::Occupied:
            return BLUE;

        case TableStatus::CallingWaiter:
            return ORANGE;

        case TableStatus::BillRequested:
            return RED;
    }

    return MUTED;
}


QString status_background(TableStatus status)
{
    switch (status)
    {
        case TableStatus::Free:
            return GREEN_LIGHT;

        case TableStatus::Occupied:
            return BLUE_LIGHT;

        case TableStatus::CallingWaiter:
            return ORANGE_LIGHT;

        case TableStatus::BillRequested:
            return RED_LIGHT;
    }

    return BG;
}


QString money(int value)
{
    return QString("%1 ֏").arg(value);
}


QPixmap rounded_pixmap(
    const QPixmap& source,
    int width,
    int height,
    int radius)
{
    if (source.isNull())
        return {};

    QPixmap scaled = source.scaled(
        width,
        height,
        Qt::KeepAspectRatioByExpanding,
        Qt::SmoothTransformation
    );

    QPixmap result(width, height);
    result.fill(Qt::transparent);

    QPainter painter(&result);
    painter.setRenderHint(QPainter::Antialiasing);

    QPainterPath path;
    path.addRoundedRect(
        0,
        0,
        width,
        height,
        radius,
        radius
    );

    painter.setClipPath(path);

    const int x =
        (scaled.width() - width) / 2;

    const int y =
        (scaled.height() - height) / 2;

    painter.drawPixmap(
        -x,
        -y,
        scaled
    );

    return result;
}


QString zone_image(const QString& zone_name)
{
    if (zone_name == "Main Hall")
        return ":/images/images/main_hall.jpg";

    if (zone_name == "Terrace")
        return ":/images/images/terrace.jpg";

    if (zone_name == "VIP Room")
        return ":/images/images/vip_room.jpg";

    if (zone_name == "Private Room")
        return ":/images/images/private_room.jpg";

    if (zone_name == "Bar Area")
        return ":/images/images/bar_area.jpg";

    return {};
}


// ------------------------------------------------------------
// Small reusable UI components
// ------------------------------------------------------------

QLabel* make_label(
    const QString& text,
    int size,
    const QString& color,
    QFont::Weight weight = QFont::Normal)
{
    auto* label = new QLabel(text);

    QFont font;
    font.setFamilies({
        "Inter",
        "Segoe UI",
        "Arial"
    });

    font.setPixelSize(size);
    font.setWeight(weight);

    label->setFont(font);

    label->setStyleSheet(
        QString(
            "color: %1;"
        ).arg(color)
    );

    return label;
}


QPushButton* make_button(
    const QString& text,
    const QString& background,
    const QString& foreground,
    const QString& border = "transparent")
{
    auto* button =
        new QPushButton(text);

    button->setCursor(
        Qt::PointingHandCursor
    );

    button->setMinimumHeight(44);

    QFont font;
    font.setFamilies({
        "Inter",
        "Segoe UI",
        "Arial"
    });

    font.setPixelSize(14);
    font.setWeight(QFont::DemiBold);

    button->setFont(font);

    button->setStyleSheet(
        QString(
            "QPushButton {"
            "  background: %1;"
            "  color: %2;"
            "  border: 1px solid %3;"
            "  border-radius: 10px;"
            "  padding: 0 18px;"
            "}"
            "QPushButton:hover {"
            "  background: %4;"
            "}"
            "QPushButton:pressed {"
            "  background: %5;"
            "}"
            "QPushButton:disabled {"
            "  background: #dddddf;"
            "  color: #999999;"
            "  border-color: #dddddf;"
            "}"
        ).arg(
            background,
            foreground,
            border,
            CARD_HOVER,
            CARD_PRESSED
        )
    );

    return button;
}


QFrame* make_card()
{
    auto* card = new QFrame;

    card->setObjectName("card");

    card->setStyleSheet(
        QString(
            "QFrame#card {"
            "  background: %1;"
            "  border: 1px solid %2;"
            "  border-radius: 16px;"
            "}"
        ).arg(
            CARD,
            BORDER
        )
    );

    return card;
}


QLabel* make_status_badge(TableStatus status)
{
    auto* badge =
        new QLabel(
            status_text(status)
        );

    badge->setAlignment(
        Qt::AlignCenter
    );

    QFont font;
    font.setFamilies({
        "Inter",
        "Segoe UI",
        "Arial"
    });

    font.setPixelSize(11);
    font.setWeight(QFont::Bold);

    badge->setFont(font);

    badge->setStyleSheet(
        QString(
            "QLabel {"
            "  color: %1;"
            "  background: %2;"
            "  border-radius: 7px;"
            "  padding: 5px 9px;"
            "}"
        ).arg(
            status_color(status),
            status_background(status)
        )
    );

    return badge;
}


// ------------------------------------------------------------
// Custom table visual
// ------------------------------------------------------------

class TableVisual : public QWidget
{
public:
    explicit TableVisual(
        TableStatus status,
        int seats,
        QWidget* parent = nullptr)
        : QWidget(parent),
          status_(status),
          seats_(seats)
    {
        setMinimumSize(
            130,
            110
        );

        setMaximumSize(
            170,
            130
        );
    }

protected:
    void paintEvent(QPaintEvent*) override
    {
        QPainter painter(this);

        painter.setRenderHint(
            QPainter::Antialiasing
        );

        const QRectF area = rect();

        const QColor status_color_value(
            status_color(status_)
        );

        const qreal cx =
            area.center().x();

        const qreal cy =
            area.center().y();

        const qreal table_w = 72;
        const qreal table_h = 48;

        const QRectF table_rect(
            cx - table_w / 2,
            cy - table_h / 2,
            table_w,
            table_h
        );

        // ----------------------------------------------------
        // Table
        // ----------------------------------------------------

        painter.setBrush(
            QColor("#ffffff")
        );

        painter.setPen(
            QPen(
                QColor(BORDER),
                1.0
            )
        );

        painter.drawRoundedRect(
            table_rect,
            10,
            10
        );

        // ----------------------------------------------------
        // Chairs
        // ----------------------------------------------------

        painter.setPen(Qt::NoPen);
        painter.setBrush(
            status_color_value
        );

        const qreal chair_w = 28;
        const qreal chair_h = 8;
        const qreal gap = 7;

        const int horizontal_chairs =
            std::min(
                seats_,
                4
            );

        const int remaining =
            std::max(
                0,
                seats_ - horizontal_chairs
            );

        const int side_chairs =
            std::min(
                remaining,
                4
            );

        // Top / bottom
        for (int i = 0;
             i < horizontal_chairs;
             ++i)
        {
            const qreal spacing =
                (
                    table_w -
                    horizontal_chairs * chair_w
                ) /
                (
                    horizontal_chairs + 1
                );

            const qreal x =
                table_rect.left() +
                spacing +
                i * (
                    chair_w +
                    spacing
                );

            painter.drawRoundedRect(
                QRectF(
                    x,
                    table_rect.top()
                        - gap
                        - chair_h,
                    chair_w,
                    chair_h
                ),
                4,
                4
            );

            painter.drawRoundedRect(
                QRectF(
                    x,
                    table_rect.bottom()
                        + gap,
                    chair_w,
                    chair_h
                ),
                4,
                4
            );
        }

        // Left / right
        for (int i = 0;
             i < side_chairs;
             ++i)
        {
            const qreal spacing =
                (
                    table_h -
                    side_chairs * chair_w
                ) /
                (
                    side_chairs + 1
                );

            const qreal y =
                table_rect.top() +
                spacing +
                i * (
                    chair_w +
                    spacing
                );

            painter.drawRoundedRect(
                QRectF(
                    table_rect.left()
                        - gap
                        - chair_h,
                    y,
                    chair_h,
                    chair_w
                ),
                4,
                4
            );

            painter.drawRoundedRect(
                QRectF(
                    table_rect.right()
                        + gap,
                    y,
                    chair_h,
                    chair_w
                ),
                4,
                4
            );
        }

        // Center status dot
        painter.setBrush(
            status_color_value
        );

        painter.drawEllipse(
            QPointF(cx, cy),
            5,
            5
        );
    }

private:
    TableStatus status_;
    int seats_;
};

} // namespace


// ============================================================
// MainWindow
// ============================================================

MainWindow::MainWindow(
    Restaurant& restaurant,
    QWidget* parent)
    : QMainWindow(parent),
      restaurant(restaurant),
      content(nullptr),
      clock_label(nullptr)
{
    setWindowTitle(
        "CTRL + EAT"
    );

    resize(
        1480,
        900
    );

    setMinimumSize(
        1180,
        720
    );

    setStyleSheet(
        QString(
            "QMainWindow {"
            "  background: %1;"
            "}"
            "QScrollArea {"
            "  border: none;"
            "  background: transparent;"
            "}"
            "QScrollBar:vertical {"
            "  width: 8px;"
            "  background: transparent;"
            "}"
            "QScrollBar::handle:vertical {"
            "  background: #c6c9c8;"
            "  border-radius: 4px;"
            "  min-height: 40px;"
            "}"
            "QScrollBar::add-line:vertical,"
            "QScrollBar::sub-line:vertical {"
            "  height: 0px;"
            "}"
        ).arg(BG)
    );

    auto* central =
        new QWidget;

    auto* root =
        new QHBoxLayout(central);

    root->setContentsMargins(
        0,
        0,
        0,
        0
    );

    root->setSpacing(0);

    // ========================================================
    // SIDEBAR
    // ========================================================

    auto* sidebar =
        new QFrame;

    sidebar->setFixedWidth(
        250
    );

    sidebar->setStyleSheet(
        QString(
            "QFrame {"
            "  background: %1;"
            "}"
        ).arg(SIDEBAR)
    );

    auto* sidebar_layout =
        new QVBoxLayout(sidebar);

    sidebar_layout->setContentsMargins(
        22,
        26,
        18,
        22
    );

    sidebar_layout->setSpacing(0);

    auto* logo =
        make_label(
            "CTRL + EAT",
            22,
            WHITE,
            QFont::Bold
        );

    sidebar_layout->addWidget(
        logo
    );

    auto* subtitle =
        make_label(
            "RESTAURANT POS",
            10,
            "#777777",
            QFont::DemiBold
        );

    subtitle->setContentsMargins(
        1,
        5,
        0,
        0
    );

    sidebar_layout->addWidget(
        subtitle
    );

    sidebar_layout->addSpacing(
        42
    );

    auto* operations =
        make_label(
            "OPERATIONS",
            10,
            "#686868",
            QFont::Bold
        );

    operations->setContentsMargins(
        3,
        0,
        0,
        10
    );

    sidebar_layout->addWidget(
        operations
    );

    auto* dashboard_button =
        new QPushButton(
            "Dashboard"
        );

    auto* zones_button =
        new QPushButton(
            "Zones"
        );

    const QList<QPushButton*> navigation_buttons = {
        dashboard_button,
        zones_button
    };

    for (auto* button :
         navigation_buttons)
    {
        button->setCursor(
            Qt::PointingHandCursor
        );

        button->setFixedHeight(
            46
        );

        QFont font;

        font.setFamilies({
            "Inter",
            "Segoe UI",
            "Arial"
        });

        font.setPixelSize(
            14
        );

        font.setWeight(
            QFont::DemiBold
        );

        button->setFont(
            font
        );

        button->setStyleSheet(
            QString(
                "QPushButton {"
                "  color: #9a9a9a;"
                "  background: transparent;"
                "  border: none;"
                "  border-radius: 10px;"
                "  padding-left: 14px;"
                "}"
                "QPushButton:hover {"
                "  color: #ffffff;"
                "  background: #151515;"
                "}"
                "QPushButton:pressed {"
                "  background: #1d1d1d;"
                "}"
            )
        );

        sidebar_layout->addWidget(
            button
        );

        sidebar_layout->addSpacing(
            3
        );
    }

    sidebar_layout->addSpacing(
        28
    );

    auto* management =
        make_label(
            "MANAGEMENT",
            10,
            "#686868",
            QFont::Bold
        );

    management->setContentsMargins(
        3,
        0,
        0,
        10
    );

    sidebar_layout->addWidget(
        management
    );

    auto* menu_button =
        new QPushButton(
            "Menu"
        );

    auto* tables_button =
        new QPushButton(
            "Tables"
        );

    const QList<QPushButton*> management_buttons = {
        menu_button,
        tables_button
    };

    for (auto* button :
         management_buttons)
    {
        button->setCursor(
            Qt::PointingHandCursor
        );

        button->setFixedHeight(
            46
        );

        QFont font;

        font.setFamilies({
            "Inter",
            "Segoe UI",
            "Arial"
        });

        font.setPixelSize(
            14
        );

        font.setWeight(
            QFont::DemiBold
        );

        button->setFont(
            font
        );

        button->setStyleSheet(
            QString(
                "QPushButton {"
                "  color: #777777;"
                "  background: transparent;"
                "  border: none;"
                "  border-radius: 10px;"
                "  padding-left: 14px;"
                "}"
                "QPushButton:hover {"
                "  color: #aaaaaa;"
                "  background: #111111;"
                "}"
            )
        );

        sidebar_layout->addWidget(
            button
        );

        sidebar_layout->addSpacing(
            3
        );
    }

    menu_button->setEnabled(
        false
    );

    tables_button->setEnabled(
        false
    );

    menu_button->setToolTip(
        "Menu management will be available here."
    );

    tables_button->setToolTip(
        "Tables are managed from their respective zones."
    );

    sidebar_layout->addStretch();

    // ========================================================
    // SYSTEM STATUS
    // ========================================================

    auto* status_frame =
        new QFrame;

    status_frame->setStyleSheet(
        "QFrame {"
        "  background: #101010;"
        "  border: 1px solid #1f1f1f;"
        "  border-radius: 12px;"
        "}"
    );

    auto* status_layout =
        new QHBoxLayout(
            status_frame
        );

    status_layout->setContentsMargins(
        13,
        11,
        13,
        11
    );

    status_layout->setSpacing(
        9
    );

    auto* online_dot =
        new QLabel;

    online_dot->setFixedSize(
        8,
        8
    );

    online_dot->setStyleSheet(
        QString(
            "QLabel {"
            "  background: %1;"
            "  border-radius: 4px;"
            "}"
        ).arg(GREEN)
    );

    status_layout->addWidget(
        online_dot
    );

    auto* online_text =
        make_label(
            "System Online",
            11,
            "#aaaaaa",
            QFont::DemiBold
        );

    status_layout->addWidget(
        online_text
    );

    status_layout->addStretch();

    sidebar_layout->addWidget(
        status_frame
    );

    root->addWidget(
        sidebar
    );

    // ========================================================
    // CONTENT AREA
    // ========================================================

    auto* content_container =
        new QWidget;

    auto* content_layout =
        new QVBoxLayout(
            content_container
        );

    content_layout->setContentsMargins(
        0,
        0,
        0,
        0
    );

    content_layout->setSpacing(
        0
    );

    // ========================================================
    // TOP BAR
    // ========================================================

    auto* top_bar =
        new QFrame;

    top_bar->setFixedHeight(
        58
    );

    top_bar->setStyleSheet(
        QString(
            "QFrame {"
            "  background: %1;"
            "  border-bottom: 1px solid %2;"
            "}"
        ).arg(
            BG,
            BORDER
        )
    );

    auto* top_bar_layout =
        new QHBoxLayout(
            top_bar
        );

    top_bar_layout->setContentsMargins(
        28,
        0,
        28,
        0
    );

    top_bar_layout->setSpacing(
        12
    );

    // --------------------------------------------------------
    // Top bar left
    // --------------------------------------------------------

    auto* system_label =
        make_label(
            "CTRL + EAT",
            12,
            MUTED,
            QFont::DemiBold
        );

    top_bar_layout->addWidget(
        system_label
    );

    top_bar_layout->addStretch();

    // --------------------------------------------------------
    // Clock container
    // --------------------------------------------------------

    auto* clock_container =
        new QFrame;

    clock_container->setStyleSheet(
        QString(
            "QFrame {"
            "  background: %1;"
            "  border: 1px solid %2;"
            "  border-radius: 10px;"
            "}"
        ).arg(
            WHITE,
            BORDER
        )
    );

    auto* clock_layout =
        new QHBoxLayout(
            clock_container
        );

    clock_layout->setContentsMargins(
        14,
        7,
        14,
        7
    );

    clock_layout->setSpacing(
        8
    );

    // --------------------------------------------------------
    // Clock status dot
    // --------------------------------------------------------

    auto* clock_dot =
        new QLabel;

    clock_dot->setFixedSize(
        7,
        7
    );

    clock_dot->setStyleSheet(
        QString(
            "QLabel {"
            "  background: %1;"
            "  border-radius: 4px;"
            "}"
        ).arg(GREEN)
    );

    clock_layout->addWidget(
        clock_dot
    );

    // --------------------------------------------------------
    // Clock label
    // --------------------------------------------------------

    clock_label =
        make_label(
            "",
            14,
            TEXT,
            QFont::DemiBold
        );

    clock_label->setMinimumWidth(
        72
    );

    clock_label->setAlignment(
        Qt::AlignCenter
    );

    clock_layout->addWidget(
        clock_label
    );

    top_bar_layout->addWidget(
        clock_container
    );

    content_layout->addWidget(
        top_bar
    );

    // ========================================================
    // STACKED PAGE CONTENT
    // ========================================================

    content =
        new QStackedWidget;

    content->setStyleSheet(
        "QStackedWidget {"
        "  background: transparent;"
        "}"
    );

    content_layout->addWidget(
        content,
        1
    );

    root->addWidget(
        content_container,
        1
    );

    // ========================================================
    // LIVE CLOCK
    // ========================================================

    auto update_clock =
    [this]()
    {
        const QDateTime now =
            QDateTime::currentDateTimeUtc()
                .toTimeZone(
                    QTimeZone(
                        "Asia/Yerevan"
                    )
                );

        clock_label->setText(
            now.toString(
                "dd MMM yyyy   HH:mm:ss"
            ).toUpper()
        );
    };

    // Initial update
    update_clock();

    // Timer
    auto* clock_timer =
        new QTimer(
            this
        );

    connect(
        clock_timer,
        &QTimer::timeout,
        this,
        update_clock
    );

    clock_timer->start(
        1000
    );

    // ========================================================
    // NAVIGATION
    // ========================================================

    connect(
        zones_button,
        &QPushButton::clicked,
        this,
        &MainWindow::showZonesPage
    );

    connect(
        dashboard_button,
        &QPushButton::clicked,
        this,
        [this]()
        {
            showZonesPage();
        }
    );

    // ========================================================
    // INITIAL PAGE
    // ========================================================

    setCentralWidget(
        central
    );

    showZonesPage();
}

// ============================================================
// Zones page
// ============================================================

QWidget* MainWindow::createZonesPage()
{
    auto* page =
        new QWidget;

    auto* outer =
        new QVBoxLayout(page);

    outer->setContentsMargins(
        42, 34, 42, 34
    );

    outer->setSpacing(0);

    auto* header =
        new QHBoxLayout;

    header->setSpacing(20);

    auto* title_block =
        new QVBoxLayout;

    title_block->setSpacing(5);

    title_block->addWidget(
        make_label(
            "Zones",
            30,
            TEXT,
            QFont::Bold
        )
    );

    title_block->addWidget(
        make_label(
            "Select a dining area to view its tables.",
            14,
            MUTED
        )
    );

    header->addLayout(
        title_block
    );

    header->addStretch();

    auto* zones_info =
        make_card();

    zones_info->setFixedWidth(
        170
    );

    zones_info->setFixedHeight(
        68
    );

    auto* zones_info_layout =
        new QVBoxLayout(zones_info);

    zones_info_layout->setContentsMargins(
        16, 11, 16, 11
    );

    zones_info_layout->setSpacing(1);

    zones_info_layout->addWidget(
        make_label(
            QString::number(
                static_cast<int>(
                    restaurant
                        .get_zones()
                        .size()
                )
            ),
            22,
            TEXT,
            QFont::Bold
        )
    );

    zones_info_layout->addWidget(
        make_label(
            "DINING ZONES",
            9,
            MUTED,
            QFont::Bold
        )
    );

    header->addWidget(
        zones_info
    );

    outer->addLayout(
        header
    );

    outer->addSpacing(
        32
    );

    auto* scroll =
        new QScrollArea;

    scroll->setWidgetResizable(
        true
    );

    scroll->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    auto* container =
        new QWidget;

    auto* grid =
        new QGridLayout(container);

    grid->setContentsMargins(
        0, 0, 8, 0
    );

    grid->setHorizontalSpacing(
        22
    );

    grid->setVerticalSpacing(
        22
    );

    const auto& zones =
        restaurant.get_zones();

    for (
        int i = 0;
        i < static_cast<int>(
                zones.size()
            );
        ++i)
    {
        const Zone& zone =
            zones.at(i);

        auto* card =
            new QFrame;

        card->setObjectName(
            "zoneCard"
        );

        card->setCursor(
            Qt::PointingHandCursor
        );

        card->setMinimumHeight(
            330
        );

        card->setStyleSheet(
            QString(
                "QFrame#zoneCard {"
                "  background: %1;"
                "  border: 1px solid %2;"
                "  border-radius: 16px;"
                "}"
            ).arg(
                WHITE,
                BORDER
            )
        );

        auto* card_layout =
            new QVBoxLayout(card);

        card_layout->setContentsMargins(
            0, 0, 0, 0
        );

        card_layout->setSpacing(0);

        auto* image_label =
            new QLabel;

        image_label->setFixedHeight(
            178
        );

        image_label->setAlignment(
            Qt::AlignCenter
        );

        const QPixmap image(
            zone_image(
                QString::fromStdString(
                    zone.get_name()
                )
            )
        );

        if (!image.isNull())
        {
            image_label->setPixmap(
                rounded_pixmap(
                    image,
                    500,
                    178,
                    15
                )
            );
        }
        else
        {
            image_label->setText(
                "NO IMAGE"
            );

            image_label->setStyleSheet(
                "color: #999999;"
                "background: #eeeeee;"
            );
        }

        card_layout->addWidget(
            image_label
        );

        auto* info =
            new QVBoxLayout;

        info->setContentsMargins(
            18, 17, 18, 17
        );

        info->setSpacing(8);

        info->addWidget(
            make_label(
                QString::fromStdString(
                    zone.get_name()
                ),
                19,
                TEXT,
                QFont::Bold
            )
        );

        info->addWidget(
            make_label(
                QString("%1 tables")
                    .arg(
                        zone
                            .get_tables()
                            .size()
                    ),
                12,
                MUTED
            )
        );

        int free_count = 0;
        int occupied_count = 0;
        int waiter_count = 0;
        int bill_count = 0;

        for (
            const Table& table :
            zone.get_tables())
        {
            switch (table.get_status())
            {
                case TableStatus::Free:
                    ++free_count;
                    break;

                case TableStatus::Occupied:
                    ++occupied_count;
                    break;

                case TableStatus::CallingWaiter:
                    ++waiter_count;
                    break;

                case TableStatus::BillRequested:
                    ++bill_count;
                    break;
            }
        }

        auto* status_row =
            new QHBoxLayout;

        status_row->setSpacing(6);

        auto add_counter =
            [status_row](
                const QString& text,
                const QString& color,
                const QString& bg)
            {
                auto* label =
                    new QLabel(text);

                label->setAlignment(
                    Qt::AlignCenter
                );

                label->setStyleSheet(
                    QString(
                        "QLabel {"
                        "  color: %1;"
                        "  background: %2;"
                        "  border-radius: 6px;"
                        "  padding: 4px 7px;"
                        "}"
                    ).arg(
                        color,
                        bg
                    )
                );

                QFont font;
                font.setFamilies({
                    "Inter",
                    "Segoe UI",
                    "Arial"
                });

                font.setPixelSize(10);
                font.setWeight(
                    QFont::Bold
                );

                label->setFont(font);

                status_row->addWidget(
                    label
                );
            };

        add_counter(
            QString("F %1")
                .arg(free_count),
            GREEN,
            GREEN_LIGHT
        );

        add_counter(
            QString("O %1")
                .arg(occupied_count),
            BLUE,
            BLUE_LIGHT
        );

        if (waiter_count > 0)
        {
            add_counter(
                QString("W %1")
                    .arg(waiter_count),
                ORANGE,
                ORANGE_LIGHT
            );
        }

        if (bill_count > 0)
        {
            add_counter(
                QString("B %1")
                    .arg(bill_count),
                RED,
                RED_LIGHT
            );
        }

        status_row->addStretch();

        info->addLayout(
            status_row
        );

        card_layout->addLayout(
            info
        );

        const QString zone_name_value =
            QString::fromStdString(
                zone.get_name()
            );

        auto* open_button =
            new QPushButton(card);

        open_button->setGeometry(
            card->rect()
        );

        open_button->setCursor(
            Qt::PointingHandCursor
        );

        open_button->setStyleSheet(
            "QPushButton {"
            "  background: transparent;"
            "  border: none;"
            "}"
        );

        connect(
            open_button,
            &QPushButton::clicked,
            this,
            [this, zone_name_value]()
            {
                Zone* zone_ptr =
                    restaurant
                        .get_zone_by_name(
                            zone_name_value
                                .toStdString()
                        );

                if (zone_ptr)
                    showTablesPage(
                        *zone_ptr
                    );
            }
        );

        connect(
            card,
            &QFrame::destroyed,
            open_button,
            &QPushButton::deleteLater
        );

        grid->addWidget(
            card,
            i / 3,
            i % 3
        );
    }

    grid->setRowStretch(
        (zones.size() + 2) / 3,
        1
    );

    scroll->setWidget(
        container
    );

    outer->addWidget(
        scroll,
        1
    );

    return page;
}


// ============================================================
// Tables page
// ============================================================

QWidget* MainWindow::createTablesPage(
    Zone& zone)
{
    auto* page =
        new QWidget;

    auto* outer =
        new QVBoxLayout(page);

    outer->setContentsMargins(
        42, 32, 42, 32
    );

    outer->setSpacing(0);

    auto* top =
        new QHBoxLayout;

    top->setSpacing(14);

    auto* back_button =
        make_button(
            "Back",
            WHITE,
            TEXT,
            BORDER
        );

    back_button->setFixedWidth(
        86
    );

    back_button->setMinimumHeight(
        38
    );

    connect(
        back_button,
        &QPushButton::clicked,
        this,
        &MainWindow::showZonesPage
    );

    top->addWidget(
        back_button
    );

    top->addSpacing(5);

    auto* zone_title =
        new QVBoxLayout;

    zone_title->setSpacing(2);

    zone_title->addWidget(
        make_label(
            QString::fromStdString(
                zone.get_name()
            ),
            25,
            TEXT,
            QFont::Bold
        )
    );

    zone_title->addWidget(
        make_label(
            QString("%1 tables")
                .arg(
                    zone.get_tables().size()
                ),
            12,
            MUTED
        )
    );

    top->addLayout(
        zone_title
    );

    top->addStretch();

    int free_count = 0;

    for (
        const Table& table :
        zone.get_tables())
    {
        if (
            table.get_status() ==
            TableStatus::Free
        )
        {
            ++free_count;
        }
    }

    auto* free_label =
        new QLabel(
            QString("%1 free")
                .arg(free_count)
        );

    free_label->setAlignment(
        Qt::AlignCenter
    );

    free_label->setStyleSheet(
        QString(
            "QLabel {"
            "  color: %1;"
            "  background: %2;"
            "  border-radius: 8px;"
            "  padding: 7px 12px;"
            "}"
        ).arg(
            GREEN,
            GREEN_LIGHT
        )
    );

    QFont free_font;
    free_font.setFamilies({
        "Inter",
        "Segoe UI",
        "Arial"
    });

    free_font.setPixelSize(11);
    free_font.setWeight(
        QFont::Bold
    );

    free_label->setFont(
        free_font
    );

    top->addWidget(
        free_label
    );

    outer->addLayout(
        top
    );

    outer->addSpacing(
        25
    );

    auto* filter_card =
        new QFrame;

    filter_card->setStyleSheet(
        QString(
            "QFrame {"
            "  background: %1;"
            "  border: 1px solid %2;"
            "  border-radius: 12px;"
            "}"
        ).arg(
            WHITE,
            BORDER
        )
    );

    auto* filter_layout =
        new QHBoxLayout(filter_card);

    filter_layout->setContentsMargins(
        7, 7, 7, 7
    );

    filter_layout->setSpacing(4);

    const QStringList filters = {
        "ALL",
        "FREE",
        "OCCUPIED",
        "WAITER",
        "BILL"
    };

    auto* grid_container =
        new QWidget;

    auto* table_grid =
        new QGridLayout(
            grid_container
        );

    table_grid->setContentsMargins(
        0, 24, 8, 0
    );

    table_grid->setHorizontalSpacing(
        18
    );

    table_grid->setVerticalSpacing(
        18
    );

    auto* scroll =
        new QScrollArea;

    scroll->setWidgetResizable(
        true
    );

    scroll->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    scroll->setWidget(
        grid_container
    );

    for (
        int i = 0;
        i < static_cast<int>(
                zone.get_tables().size()
            );
        ++i)
    {
        const Table& table =
            zone.get_tables().at(i);

        auto* card =
            new QFrame;

        card->setObjectName(
            "tableCard"
        );

        card->setMinimumHeight(
            255
        );

        card->setStyleSheet(
            QString(
                "QFrame#tableCard {"
                "  background: %1;"
                "  border: 1px solid %2;"
                "  border-radius: 16px;"
                "}"
            ).arg(
                WHITE,
                BORDER
            )
        );

        auto* layout =
            new QVBoxLayout(card);

        layout->setContentsMargins(
            18, 16, 18, 16
        );

        layout->setSpacing(7);

        auto* header =
            new QHBoxLayout;

        header->addWidget(
            make_label(
                QString("TABLE %1")
                    .arg(
                        table.get_table_number(),
                        2,
                        10,
                        QChar('0')
                    ),
                13,
                TEXT,
                QFont::Bold
            )
        );

        header->addStretch();

        header->addWidget(
            make_status_badge(
                table.get_status()
            )
        );

        layout->addLayout(
            header
        );

        auto* visual_layout =
            new QHBoxLayout;

        visual_layout->setContentsMargins(
            0, 4, 0, 0
        );

        auto* visual =
            new TableVisual(
                table.get_status(),
                table.get_chairs_count()
            );

        visual_layout->addStretch();

        visual_layout->addWidget(
            visual
        );

        visual_layout->addStretch();

        layout->addLayout(
            visual_layout
        );

        auto* details =
            new QHBoxLayout;

        details->addWidget(
            make_label(
                QString("%1 seats")
                    .arg(
                        table.get_chairs_count()
                    ),
                11,
                MUTED
            )
        );

        details->addStretch();

        if (
            table.get_status() !=
            TableStatus::Free
        )
        {
            details->addWidget(
                make_label(
                    QString("%1 guests")
                        .arg(
                            table.get_clients_count()
                        ),
                    11,
                    TEXT,
                    QFont::DemiBold
                )
            );
        }

        layout->addLayout(
            details
        );

        auto* open =
            new QPushButton(card);

        open->setGeometry(
            card->rect()
        );

        open->setCursor(
            Qt::PointingHandCursor
        );

        open->setStyleSheet(
            "QPushButton {"
            "  background: transparent;"
            "  border: none;"
            "}"
        );

        const int table_number =
            table.get_table_number();

        const QString zone_name =
            QString::fromStdString(
                zone.get_name()
            );

        connect(
            open,
            &QPushButton::clicked,
            this,
            [this, zone_name, table_number]()
            {
                Zone* zone_ptr =
                    restaurant
                        .get_zone_by_name(
                            zone_name
                                .toStdString()
                        );

                if (!zone_ptr)
                    return;

                Table* table_ptr =
                    zone_ptr
                        ->get_table_by_number(
                            table_number
                        );

                if (table_ptr)
                {
                    showTablePage(
                        *zone_ptr,
                        *table_ptr
                    );
                }
            }
        );

        table_grid->addWidget(
            card,
            i / 4,
            i % 4
        );
    }

    outer->addWidget(
        scroll,
        1
    );

    for (
        int i = 0;
        i < filters.size();
        ++i)
    {
        auto* filter =
            new QPushButton(
                filters.at(i)
            );

        filter->setFixedHeight(
            34
        );

        filter->setMinimumWidth(
            72
        );

        filter->setCursor(
            Qt::PointingHandCursor
        );

        filter->setStyleSheet(
            QString(
                "QPushButton {"
                "  color: %1;"
                "  background: transparent;"
                "  border: none;"
                "  border-radius: 8px;"
                "  padding: 0 12px;"
                "}"
                "QPushButton:hover {"
                "  background: %2;"
                "}"
            ).arg(
                i == 0
                    ? TEXT
                    : MUTED,
                i == 0
                    ? "#eeeeee"
                    : "#f4f4f4"
            )
        );

        filter_layout->addWidget(
            filter
        );
    }

    filter_layout->addStretch();

    outer->insertWidget(
        2,
        filter_card
    );

    return page;
}


// ============================================================
// Table detail page
// ============================================================

QWidget* MainWindow::createTablePage(
    Zone& zone,
    Table& table)
{
    auto* page =
        new QWidget;

    auto* outer =
        new QVBoxLayout(page);

    outer->setContentsMargins(
        42, 32, 42, 32
    );

    outer->setSpacing(0);

    // ========================================================
    // HEADER
    // ========================================================

    auto* header =
        new QHBoxLayout;

    header->setSpacing(0);

    auto* back_button =
        make_button(
            "Back",
            WHITE,
            TEXT,
            BORDER
        );

    back_button->setFixedWidth(
        86
    );

    back_button->setMinimumHeight(
        38
    );

    const QString zone_name =
        QString::fromStdString(
            zone.get_name()
        );

    const int table_number =
        table.get_table_number();

    connect(
        back_button,
        &QPushButton::clicked,
        this,
        [this, zone_name]()
        {
            Zone* zone_ptr =
                restaurant
                    .get_zone_by_name(
                        zone_name.toStdString()
                    );

            if (zone_ptr)
            {
                showTablesPage(
                    *zone_ptr
                );
            }
        }
    );

    header->addWidget(
        back_button
    );

    header->addSpacing(
        16
    );

    auto* title =
        new QVBoxLayout;

    title->setSpacing(2);

    title->addWidget(
        make_label(
            QString("Table %1")
                .arg(
                    table_number,
                    2,
                    10,
                    QChar('0')
                ),
            26,
            TEXT,
            QFont::Bold
        )
    );

    title->addWidget(
        make_label(
            zone_name,
            12,
            MUTED
        )
    );

    header->addLayout(
        title
    );

    header->addStretch();

    header->addWidget(
        make_status_badge(
            table.get_status()
        )
    );

    outer->addLayout(
        header
    );

    outer->addSpacing(
        24
    );

    // ========================================================
    // MAIN
    //
    // LEFT  = TABLE OVERVIEW
    // RIGHT = CURRENT ORDER
    // ========================================================

    auto* main =
        new QHBoxLayout;

    main->setSpacing(
        22
    );

    // ========================================================
    // LEFT — TABLE OVERVIEW
    // ========================================================

    auto* table_card =
        make_card();

    table_card->setFixedWidth(
        340
    );

    auto* table_layout =
        new QVBoxLayout(
            table_card
        );

    table_layout->setContentsMargins(
        22,
        20,
        22,
        20
    );

    table_layout->setSpacing(
        12
    );

    // --------------------------------------------------------
    // Table overview header
    // --------------------------------------------------------

    auto* table_header =
        new QHBoxLayout;

    table_header->addWidget(
        make_label(
            "TABLE OVERVIEW",
            11,
            MUTED,
            QFont::Bold
        )
    );

    table_header->addStretch();

    table_header->addWidget(
        make_status_badge(
            table.get_status()
        )
    );

    table_layout->addLayout(
        table_header
    );

    // --------------------------------------------------------
    // Compact table visual
    // --------------------------------------------------------

    auto* visual_box =
        new QWidget;

    visual_box->setFixedHeight(
        145
    );

    auto* visual_box_layout =
        new QVBoxLayout(
            visual_box
        );

    visual_box_layout->setContentsMargins(
        0,
        2,
        0,
        2
    );

    auto* visual =
        new TableVisual(
            table.get_status(),
            table.get_chairs_count()
        );

    visual->setMinimumSize(
        150,
        120
    );

    visual->setMaximumSize(
        170,
        130
    );

    visual_box_layout->addWidget(
        visual,
        0,
        Qt::AlignCenter
    );

    table_layout->addWidget(
        visual_box
    );

    // --------------------------------------------------------
    // Table identity
    // --------------------------------------------------------

    auto* identity =
        make_label(
            QString("TABLE %1")
                .arg(
                    table_number,
                    2,
                    10,
                    QChar('0')
                ),
            19,
            TEXT,
            QFont::Bold
        );

    identity->setAlignment(
        Qt::AlignCenter
    );

    table_layout->addWidget(
        identity
    );

    auto* zone_label =
        make_label(
            zone_name,
            11,
            MUTED
        );

    zone_label->setAlignment(
        Qt::AlignCenter
    );

    table_layout->addWidget(
        zone_label
    );

    // --------------------------------------------------------
    // Seats / Guests
    // --------------------------------------------------------

    auto* stats =
        new QHBoxLayout;

    stats->setSpacing(
        10
    );

    auto* seats_box =
        new QFrame;

    seats_box->setStyleSheet(
        QString(
            "QFrame {"
            "  background: %1;"
            "  border: 1px solid %2;"
            "  border-radius: 10px;"
            "}"
        ).arg(
            WHITE,
            BORDER
        )
    );

    auto* seats_layout =
        new QVBoxLayout(
            seats_box
        );

    seats_layout->setContentsMargins(
        12,
        8,
        12,
        8
    );

    seats_layout->setSpacing(1);

    seats_layout->addWidget(
        make_label(
            QString::number(
                table.get_chairs_count()
            ),
            16,
            TEXT,
            QFont::Bold
        )
    );

    seats_layout->addWidget(
        make_label(
            "SEATS",
            9,
            MUTED,
            QFont::Bold
        )
    );

    stats->addWidget(
        seats_box,
        1
    );

    auto* guests_box =
        new QFrame;

    guests_box->setStyleSheet(
        QString(
            "QFrame {"
            "  background: %1;"
            "  border: 1px solid %2;"
            "  border-radius: 10px;"
            "}"
        ).arg(
            WHITE,
            BORDER
        )
    );

    auto* guests_layout =
        new QVBoxLayout(
            guests_box
        );

    guests_layout->setContentsMargins(
        12,
        8,
        12,
        8
    );

    guests_layout->setSpacing(1);

    guests_layout->addWidget(
        make_label(
            QString::number(
                table.get_clients_count()
            ),
            16,
            TEXT,
            QFont::Bold
        )
    );

    guests_layout->addWidget(
        make_label(
            "GUESTS",
            9,
            MUTED,
            QFont::Bold
        )
    );

    stats->addWidget(
        guests_box,
        1
    );

    table_layout->addLayout(
        stats
    );

    // --------------------------------------------------------
    // Divider
    // --------------------------------------------------------

    auto* action_separator =
        new QFrame;

    action_separator->setFrameShape(
        QFrame::HLine
    );

    action_separator->setFrameShadow(
        QFrame::Plain
    );

    action_separator->setStyleSheet(
        QString(
            "QFrame {"
            "  color: %1;"
            "  background: %1;"
            "  max-height: 1px;"
            "}"
        ).arg(BORDER)
    );

    table_layout->addWidget(
        action_separator
    );

    // --------------------------------------------------------
    // ACTIONS
    // --------------------------------------------------------

    table_layout->addWidget(
        make_label(
            "ACTIONS",
            10,
            MUTED,
            QFont::Bold
        )
    );

    auto* actions_grid =
        new QGridLayout;

    actions_grid->setHorizontalSpacing(
        8
    );

    actions_grid->setVerticalSpacing(
        8
    );

    // ========================================================
    // FREE TABLE
    // ========================================================

    if (
        table.get_status() ==
        TableStatus::Free
    )
    {
        auto* guest_title =
            make_label(
                "NUMBER OF GUESTS",
                9,
                MUTED,
                QFont::Bold
            );

        guest_title->setAlignment(
            Qt::AlignCenter
        );

        table_layout->addWidget(
            guest_title
        );

        auto* guests_value =
            make_label(
                "",
                25,
                TEXT,
                QFont::Bold
            );

        guests_value->setAlignment(
            Qt::AlignCenter
        );

        guests_value->setFixedHeight(
            42
        );

        guests_value->setStyleSheet(
            QString(
                "QLabel {"
                "  background: %1;"
                "  border: 1px solid %2;"
                "  border-radius: 10px;"
                "}"
            ).arg(
                WHITE,
                BORDER
            )
        );

        table_layout->addWidget(
            guests_value
        );

        // ----------------------------------------------------
        // Numeric keypad
        // ----------------------------------------------------

        auto* keypad =
            new QGridLayout;

        keypad->setSpacing(
            6
        );

        const QStringList keys = {
            "1", "2", "3",
            "4", "5", "6",
            "7", "8", "9",
            "DEL", "0", "CLEAR"
        };

        for (
            int i = 0;
            i < keys.size();
            ++i)
        {
            auto* key =
                new QPushButton(
                    keys.at(i)
                );

            key->setFixedHeight(
                34
            );

            key->setCursor(
                Qt::PointingHandCursor
            );

            key->setStyleSheet(
                QString(
                    "QPushButton {"
                    "  background: %1;"
                    "  color: %2;"
                    "  border: 1px solid %3;"
                    "  border-radius: 8px;"
                    "  font-size: 11px;"
                    "  font-weight: 600;"
                    "}"
                    "QPushButton:hover {"
                    "  background: %4;"
                    "}"
                    "QPushButton:pressed {"
                    "  background: %5;"
                    "}"
                ).arg(
                    WHITE,
                    TEXT,
                    BORDER,
                    "#f0f0f0",
                    "#e9e9e9"
                )
            );

            keypad->addWidget(
                key,
                i / 3,
                i % 3
            );

            connect(
                key,
                &QPushButton::clicked,
                this,
                [key, guests_value]()
                {
                    const QString value =
                        key->text();

                    if (value == "DEL")
                    {
                        QString current =
                            guests_value->text();

                        if (!current.isEmpty())
                            current.chop(1);

                        guests_value->setText(
                            current
                        );

                        return;
                    }

                    if (value == "CLEAR")
                    {
                        guests_value->clear();
                        return;
                    }

                    QString current =
                        guests_value->text();

                    if (current == "0")
                        current.clear();

                    current += value;

                    bool ok = false;

                    const int number =
                        current.toInt(
                            &ok
                        );

                    if (!ok || number > 99)
                        return;

                    guests_value->setText(
                        current
                    );
                }
            );
        }

        table_layout->addLayout(
            keypad
        );

        // ----------------------------------------------------
        // Open table
        // ----------------------------------------------------

        auto* open_button =
            make_button(
                "OPEN TABLE",
                GREEN,
                WHITE
            );

        open_button->setMinimumHeight(
            44
        );

        connect(
            open_button,
            &QPushButton::clicked,
            this,
            [
                this,
                zone_name,
                table_number,
                guests_value
            ]()
            {
                Zone* zone_ptr =
                    restaurant
                        .get_zone_by_name(
                            zone_name.toStdString()
                        );

                if (!zone_ptr)
                    return;

                Table* table_ptr =
                    zone_ptr
                        ->get_table_by_number(
                            table_number
                        );

                if (!table_ptr)
                    return;

                bool ok = false;

                const int guests =
                    guests_value
                        ->text()
                        .toInt(&ok);

                if (!ok || guests <= 0)
                    return;

                if (
                    guests >
                    table_ptr
                        ->get_chairs_count()
                )
                {
                    return;
                }

                if (
                    table_ptr
                        ->open_table(
                            guests
                        )
                )
                {
                    showTablePage(
                        *zone_ptr,
                        *table_ptr
                    );
                }
            }
        );

        table_layout->addWidget(
            open_button
        );
    }

    // ========================================================
    // OCCUPIED / WAITER / BILL
    // ========================================================

    else
    {
        const Order* order =
            table.get_order();

        const int item_count =
            order
                ? static_cast<int>(
                    order
                        ->get_items()
                        .size()
                )
                : 0;

        const bool paid =
            order &&
            order->is_paid();

        // ----------------------------------------------------
        // CALL WAITER
        // ----------------------------------------------------

        if (
            table.get_status() ==
            TableStatus::Occupied
        )
        {
            auto* waiter_button =
                make_button(
                    "CALL WAITER",
                    ORANGE_LIGHT,
                    ORANGE,
                    "#ead9c5"
                );

            waiter_button->setMinimumHeight(
                44
            );

            connect(
                waiter_button,
                &QPushButton::clicked,
                this,
                [
                    this,
                    zone_name,
                    table_number
                ]()
                {
                    Zone* zone_ptr =
                        restaurant
                            .get_zone_by_name(
                                zone_name.toStdString()
                            );

                    if (!zone_ptr)
                        return;

                    Table* table_ptr =
                        zone_ptr
                            ->get_table_by_number(
                                table_number
                            );

                    if (!table_ptr)
                        return;

                    if (
                        table_ptr
                            ->call_waiter()
                    )
                    {
                        showTablePage(
                            *zone_ptr,
                            *table_ptr
                        );
                    }
                }
            );

            actions_grid->addWidget(
                waiter_button,
                0,
                0,
                1,
                2
            );
        }

        // ----------------------------------------------------
        // REQUEST BILL
        // ----------------------------------------------------

        if (
            (
                table.get_status() ==
                    TableStatus::Occupied ||
                table.get_status() ==
                    TableStatus::CallingWaiter
            ) &&
            item_count > 0
        )
        {
            auto* bill_button =
                make_button(
                    "REQUEST BILL",
                    RED,
                    WHITE
                );

            bill_button->setMinimumHeight(
                44
            );

            connect(
    bill_button,
    &QPushButton::clicked,
    this,
    [
        this,
        zone_name,
        table_number
    ]()
    {
        Zone* zone_ptr =
            restaurant
                .get_zone_by_name(
                    zone_name.toStdString()
                );

        if (!zone_ptr)
            return;

        Table* table_ptr =
            zone_ptr
                ->get_table_by_number(
                    table_number
                );

        if (!table_ptr)
            return;

        if (
            table_ptr
                ->request_bill()
        )
        {
            showPaymentPage(
                *zone_ptr,
                *table_ptr
            );
        }
    }
);

actions_grid->addWidget(
    bill_button,
    1,
    0,
    1,
    2
);
        }

        // ----------------------------------------------------
        // MARK AS PAID
        // ----------------------------------------------------

        if (
            table.get_status() ==
                TableStatus::BillRequested &&
            order != nullptr &&
            !order->is_paid()
        )
        {
            auto* pay_button =
                make_button(
                    "MARK AS PAID",
                    GREEN,
                    WHITE
                );

            pay_button->setMinimumHeight(
                44
            );

            connect(
                pay_button,
                &QPushButton::clicked,
                this,
                [
                    this,
                    zone_name,
                    table_number
                ]()
                {
                    Zone* zone_ptr =
                        restaurant
                            .get_zone_by_name(
                                zone_name.toStdString()
                            );

                    if (!zone_ptr)
                        return;

                    Table* table_ptr =
                        zone_ptr
                            ->get_table_by_number(
                                table_number
                            );

                    if (!table_ptr)
                        return;

                    Order* order_ptr =
                        table_ptr
                            ->get_order();

                    if (!order_ptr)
                        return;


                    {
                        showTablePage(
                            *zone_ptr,
                            *table_ptr
                        );
                    }
                }
            );

            actions_grid->addWidget(
                pay_button,
                2,
                0,
                1,
                2
            );
        }

        // ----------------------------------------------------
        // CLOSE TABLE
        // ----------------------------------------------------

        if (
            table.get_status() ==
                TableStatus::BillRequested &&
            paid
        )
        {
            auto* close_button =
                make_button(
                    "CLOSE TABLE",
                    WHITE,
                    RED,
                    RED
                );

            close_button->setMinimumHeight(
                44
            );

            connect(
                close_button,
                &QPushButton::clicked,
                this,
                [
                    this,
                    zone_name,
                    table_number
                ]()
                {
                    Zone* zone_ptr =
                        restaurant
                            .get_zone_by_name(
                                zone_name.toStdString()
                            );

                    if (!zone_ptr)
                        return;

                    Table* table_ptr =
                        zone_ptr
                            ->get_table_by_number(
                                table_number
                            );

                    if (!table_ptr)
                        return;

                    if (
                        table_ptr
                            ->close_table()
                    )
                    {
                        showTablesPage(
                            *zone_ptr
                        );
                    }
                }
            );

            actions_grid->addWidget(
                close_button,
                3,
                0,
                1,
                2
            );
        }

        table_layout->addLayout(
            actions_grid
        );

        table_layout->addStretch();
    }

    main->addWidget(
        table_card
    );

    // ========================================================
    // RIGHT — CURRENT ORDER
    // ========================================================

    auto* order_card =
        make_card();

    auto* order_layout =
        new QVBoxLayout(
            order_card
        );

    order_layout->setContentsMargins(
        28,
        24,
        28,
        24
    );

    order_layout->setSpacing(
        16
    );

    // --------------------------------------------------------
    // Order header
    // --------------------------------------------------------

    auto* order_header =
        new QHBoxLayout;

    auto* order_title =
        new QVBoxLayout;

    order_title->setSpacing(2);

    order_title->addWidget(
        make_label(
            "CURRENT ORDER",
            13,
            MUTED,
            QFont::Bold
        )
    );

    const Order* current_order =
        table.get_order();

    const int current_item_count =
        current_order
            ? static_cast<int>(
                current_order
                    ->get_items()
                    .size()
            )
            : 0;

    order_title->addWidget(
        make_label(
            QString("%1 items")
                .arg(
                    current_item_count
                ),
            11,
            MUTED
        )
    );

    order_header->addLayout(
        order_title
    );

    order_header->addStretch();

    // --------------------------------------------------------
    // ADD ITEM
    // --------------------------------------------------------

    if (
        table.get_status() !=
        TableStatus::Free
    )
    {
        auto* add_item_button =
            make_button(
                "ADD ITEM",
                GREEN,
                WHITE
            );

        add_item_button->setFixedWidth(
            125
        );

        add_item_button->setMinimumHeight(
            42
        );

        connect(
            add_item_button,
            &QPushButton::clicked,
            this,
            [
                this,
                zone_name,
                table_number
            ]()
            {
                Zone* zone_ptr =
                    restaurant
                        .get_zone_by_name(
                            zone_name.toStdString()
                        );

                if (!zone_ptr)
                    return;

                Table* table_ptr =
                    zone_ptr
                        ->get_table_by_number(
                            table_number
                        );

                if (!table_ptr)
                    return;

                showAddItemPage(
                    *zone_ptr,
                    *table_ptr
                );
            }
        );

        order_header->addWidget(
            add_item_button
        );
    }

    order_layout->addLayout(
        order_header
    );

    // --------------------------------------------------------
    // Order items
    // --------------------------------------------------------

    auto* order_scroll =
        new QScrollArea;

    order_scroll->setWidgetResizable(
        true
    );

    order_scroll->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    order_scroll->setStyleSheet(
        "QScrollArea {"
        "  border: none;"
        "  background: transparent;"
        "}"
    );

    auto* order_container =
        new QWidget;

    auto* order_items_layout =
        new QVBoxLayout(
            order_container
        );

    order_items_layout->setContentsMargins(
        0,
        0,
        8,
        0
    );

    order_items_layout->setSpacing(
        9
    );

    if (
        current_order &&
        !current_order
            ->get_items()
            .empty()
    )
    {
        for (
            const OrderItem& item :
            current_order->get_items()
        )
        {
            auto* item_card =
                new QFrame;

            item_card->setStyleSheet(
                QString(
                    "QFrame {"
                    "  background: %1;"
                    "  border: 1px solid %2;"
                    "  border-radius: 12px;"
                    "}"
                ).arg(
                    WHITE,
                    BORDER
                )
            );

            auto* item_layout =
                new QVBoxLayout(
                    item_card
                );

            item_layout->setContentsMargins(
                16,
                13,
                16,
                13
            );

            item_layout->setSpacing(
                6
            );

            auto* name_row =
                new QHBoxLayout;

            name_row->addWidget(
                make_label(
                    QString::fromStdString(
                        item.get_name()
                    ),
                    15,
                    TEXT,
                    QFont::DemiBold
                ),
                1
            );

            auto* quantity_badge =
                new QLabel(
                    QString("x%1")
                        .arg(
                            item.get_quantity()
                        )
                );

            quantity_badge->setAlignment(
                Qt::AlignCenter
            );

            quantity_badge->setStyleSheet(
                QString(
                    "QLabel {"
                    "  color: %1;"
                    "  background: %2;"
                    "  border-radius: 7px;"
                    "  padding: 4px 8px;"
                    "}"
                ).arg(
                    BLUE,
                    BLUE_LIGHT
                )
            );

            QFont quantity_font;

            quantity_font.setFamilies({
                "Inter",
                "Segoe UI",
                "Arial"
            });

            quantity_font.setPixelSize(11);
            quantity_font.setWeight(
                QFont::Bold
            );

            quantity_badge->setFont(
                quantity_font
            );

            name_row->addWidget(
                quantity_badge
            );

            item_layout->addLayout(
                name_row
            );

            auto* price_row =
                new QHBoxLayout;

            price_row->addWidget(
                make_label(
                    money(
                        item.get_unit_price()
                    ) + " each",
                    11,
                    MUTED
                )
            );

            price_row->addStretch();

            price_row->addWidget(
                make_label(
                    money(
                        item.get_subtotal()
                    ),
                    14,
                    TEXT,
                    QFont::Bold
                )
            );

            item_layout->addLayout(
                price_row
            );

            order_items_layout->addWidget(
                item_card
            );
        }
    }
    else
    {
        auto* empty_card =
            new QFrame;

        empty_card->setStyleSheet(
            QString(
                "QFrame {"
                "  background: %1;"
                "  border: 1px dashed %2;"
                "  border-radius: 12px;"
                "}"
            ).arg(
                WHITE,
                BORDER
            )
        );

        auto* empty_layout =
            new QVBoxLayout(
                empty_card
            );

        empty_layout->setContentsMargins(
            28,
            45,
            28,
            45
        );

        empty_layout->setSpacing(7);

        auto* empty_title =
            make_label(
                table.get_status() ==
                    TableStatus::Free
                    ? "No active order"
                    : "Order is empty",
                17,
                TEXT,
                QFont::DemiBold
            );

        empty_title->setAlignment(
            Qt::AlignCenter
        );

        empty_layout->addWidget(
            empty_title
        );

        auto* empty_hint =
            make_label(
                table.get_status() ==
                    TableStatus::Free
                    ? "Open the table to start an order."
                    : "Add items from the menu.",
                11,
                MUTED
            );

        empty_hint->setAlignment(
            Qt::AlignCenter
        );

        empty_hint->setWordWrap(
            true
        );

        empty_layout->addWidget(
            empty_hint
        );

        order_items_layout->addWidget(
            empty_card
        );
    }

    order_items_layout->addStretch();

    order_scroll->setWidget(
        order_container
    );

    order_layout->addWidget(
        order_scroll,
        1
    );

    // --------------------------------------------------------
    // Total separator
    // --------------------------------------------------------

    auto* total_separator =
        new QFrame;

    total_separator->setFrameShape(
        QFrame::HLine
    );

    total_separator->setFrameShadow(
        QFrame::Plain
    );

    total_separator->setStyleSheet(
        QString(
            "QFrame {"
            "  color: %1;"
            "  background: %1;"
            "  max-height: 1px;"
            "}"
        ).arg(BORDER)
    );

    order_layout->addWidget(
        total_separator
    );

    // --------------------------------------------------------
    // Total
    // --------------------------------------------------------

    const int current_total =
        current_order
            ? current_order->get_total()
            : 0;

    auto* total_row =
        new QHBoxLayout;

    total_row->setSpacing(10);

    total_row->addWidget(
        make_label(
            "TOTAL",
            12,
            MUTED,
            QFont::Bold
        )
    );

    total_row->addStretch();

    total_row->addWidget(
        make_label(
            money(current_total),
            26,
            TEXT,
            QFont::Bold
        )
    );

    order_layout->addLayout(
        total_row
    );

    main->addWidget(
        order_card,
        1
    );

    // ========================================================
    // FINAL LAYOUT
    // ========================================================

    outer->addLayout(
        main,
        1
    );

    return page;
}

// ============================================================
// ADD ITEM PAGE
// ============================================================

QWidget* MainWindow::createAddItemPage(
    Zone& zone,
    Table& table)
{
    auto* page =
        new QWidget;

    auto* outer =
        new QVBoxLayout(page);

    outer->setContentsMargins(
        42,
        32,
        42,
        32
    );

    outer->setSpacing(0);

    // ========================================================
    // HEADER
    // ========================================================

    auto* header =
        new QHBoxLayout;

    header->setSpacing(
        16
    );

    auto* back_button =
        make_button(
            "Back",
            WHITE,
            TEXT,
            BORDER
        );

    back_button->setFixedWidth(
        86
    );

    back_button->setMinimumHeight(
        38
    );

    const QString zone_name =
        QString::fromStdString(
            zone.get_name()
        );

    const int table_number =
        table.get_table_number();

    connect(
        back_button,
        &QPushButton::clicked,
        this,
        [
            this,
            zone_name,
            table_number
        ]()
        {
            Zone* zone_ptr =
                restaurant
                    .get_zone_by_name(
                        zone_name.toStdString()
                    );

            if (!zone_ptr)
                return;

            Table* table_ptr =
                zone_ptr
                    ->get_table_by_number(
                        table_number
                    );

            if (!table_ptr)
                return;

            showTablePage(
                *zone_ptr,
                *table_ptr
            );
        }
    );

    header->addWidget(
        back_button
    );

    auto* title_block =
        new QVBoxLayout;

    title_block->setSpacing(
        2
    );

    title_block->addWidget(
        make_label(
            QString(
                "TABLE %1 · ADD ITEMS"
            ).arg(
                table_number,
                2,
                10,
                QChar('0')
            ),
            26,
            TEXT,
            QFont::Bold
        )
    );

    title_block->addWidget(
        make_label(
            zone_name,
            12,
            MUTED
        )
    );

    header->addLayout(
        title_block
    );

    header->addStretch();

    header->addWidget(
        make_status_badge(
            table.get_status()
        )
    );

    header->addSpacing(
        12
    );

    const Order* header_order =
        table.get_order();

    const int header_total =
        header_order
            ? header_order->get_total()
            : 0;

    auto* total_card =
        make_card();

    total_card->setFixedWidth(
        175
    );

    total_card->setFixedHeight(
        68
    );

    auto* total_layout =
        new QVBoxLayout(
            total_card
        );

    total_layout->setContentsMargins(
        16,
        10,
        16,
        10
    );

    total_layout->setSpacing(
        1
    );

    total_layout->addWidget(
        make_label(
            money(header_total),
            20,
            TEXT,
            QFont::Bold
        )
    );

    total_layout->addWidget(
        make_label(
            "CURRENT TOTAL",
            9,
            MUTED,
            QFont::Bold
        )
    );

    header->addWidget(
        total_card
    );

    outer->addLayout(
        header
    );

    outer->addSpacing(
        24
    );

    // ========================================================
    // MAIN TWO-COLUMN AREA
    // ========================================================

    auto* main =
        new QHBoxLayout;

    main->setSpacing(
        22
    );

    // ========================================================
    // LEFT — MENU
    // ========================================================

    auto* menu_panel =
        new QFrame;

    menu_panel->setObjectName(
        "menuPanel"
    );

    menu_panel->setStyleSheet(
        QString(
            "QFrame#menuPanel {"
            "  background: %1;"
            "  border: 1px solid %2;"
            "  border-radius: 16px;"
            "}"
        ).arg(
            WHITE,
            BORDER
        )
    );

    auto* menu_layout =
        new QVBoxLayout(
            menu_panel
        );

    menu_layout->setContentsMargins(
        20,
        20,
        20,
        20
    );

    menu_layout->setSpacing(
        16
    );

    // --------------------------------------------------------
    // Menu title
    // --------------------------------------------------------

    auto* menu_header =
        new QHBoxLayout;

    menu_header->addWidget(
        make_label(
            "MENU",
            12,
            MUTED,
            QFont::Bold
        )
    );

    menu_header->addStretch();

    menu_header->addWidget(
        make_label(
            "SELECT ITEMS",
            9,
            MUTED,
            QFont::Bold
        )
    );

    menu_layout->addLayout(
        menu_header
    );

    // --------------------------------------------------------
    // Category bar
    // --------------------------------------------------------

    auto* category_card =
        new QFrame;

    category_card->setStyleSheet(
        QString(
            "QFrame {"
            "  background: %1;"
            "  border: 1px solid %2;"
            "  border-radius: 10px;"
            "}"
        ).arg(
            "#f8f8f8",
            BORDER
        )
    );

    auto* category_layout =
        new QHBoxLayout(
            category_card
        );

    category_layout->setContentsMargins(
        6,
        6,
        6,
        6
    );

    category_layout->setSpacing(
        4
    );

    const auto& categories =
        restaurant
            .get_menu()
            .get_categories();

    if (categories.empty())
    {
        category_layout->addWidget(
            make_label(
                "No categories",
                12,
                MUTED
            )
        );
    }
    else
    {
        for (
            const MenuCategory& category :
            categories)
        {
            auto* category_button =
                new QPushButton(
                    QString::fromStdString(
                        category.get_name()
                    )
                );

            category_button->setFixedHeight(
                34
            );

            category_button->setCursor(
                Qt::PointingHandCursor
            );

            category_button->setStyleSheet(
                QString(
                    "QPushButton {"
                    "  color: %1;"
                    "  background: transparent;"
                    "  border: none;"
                    "  border-radius: 8px;"
                    "  padding: 0 11px;"
                    "}"
                    "QPushButton:hover {"
                    "  color: %2;"
                    "  background: %3;"
                    "}"
                ).arg(
                    MUTED,
                    TEXT,
                    "#eeeeee"
                )
            );

            category_layout->addWidget(
                category_button
            );
        }
    }

    category_layout->addStretch();

    menu_layout->addWidget(
        category_card
    );

    // --------------------------------------------------------
    // Menu item scroll area
    // --------------------------------------------------------

    auto* menu_scroll =
        new QScrollArea;

    menu_scroll->setWidgetResizable(
        true
    );

    menu_scroll->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    auto* menu_container =
        new QWidget;

    auto* menu_items_layout =
        new QVBoxLayout(
            menu_container
        );

    menu_items_layout->setContentsMargins(
        0,
        0,
        8,
        0
    );

    menu_items_layout->setSpacing(
        10
    );

    if (categories.empty())
    {
        menu_items_layout->addWidget(
            make_label(
                "No menu items available.",
                13,
                MUTED
            )
        );
    }
    else
    {
        for (
            const MenuCategory& category :
            categories)
        {
            auto* category_heading =
                make_label(
                    QString::fromStdString(
                        category.get_name()
                    ),
                    16,
                    TEXT,
                    QFont::Bold
                );

            category_heading->setContentsMargins(
                2,
                4,
                0,
                2
            );

            menu_items_layout->addWidget(
                category_heading
            );

            const auto& items =
                category.get_items();

            for (
                const MenuItem& item :
                items)
            {
                auto* item_card =
                    new QFrame;

                item_card->setObjectName(
                    "menuItemCard"
                );

                item_card->setStyleSheet(
                    QString(
                        "QFrame#menuItemCard {"
                        "  background: %1;"
                        "  border: 1px solid %2;"
                        "  border-radius: 12px;"
                        "}"
                    ).arg(
                        "#fbfbfb",
                        BORDER
                    )
                );

                auto* item_row =
                    new QHBoxLayout(
                        item_card
                    );

                item_row->setContentsMargins(
                    15,
                    12,
                    15,
                    12
                );

                item_row->setSpacing(
                    12
                );

                auto* item_info =
                    new QVBoxLayout;

                item_info->setSpacing(
                    3
                );

                item_info->addWidget(
                    make_label(
                        QString::fromStdString(
                            item.get_name()
                        ),
                        14,
                        TEXT,
                        QFont::Bold
                    )
                );

                item_info->addWidget(
                    make_label(
                        money(
                            item.get_price()
                        ),
                        11,
                        MUTED
                    )
                );

                item_row->addLayout(
                    item_info,
                    1
                );

                auto* minus_button =
                    new QPushButton(
                        "-"
                    );

                auto* quantity_label =
                    make_label(
                        "1",
                        13,
                        TEXT,
                        QFont::Bold
                    );

                auto* plus_button =
                    new QPushButton(
                        "+"
                    );

                minus_button->setFixedSize(
                    32,
                    32
                );

                plus_button->setFixedSize(
                    32,
                    32
                );

                quantity_label->setFixedWidth(
                    24
                );

                quantity_label->setAlignment(
                    Qt::AlignCenter
                );

                const QString quantity_style =
                    QString(
                        "QPushButton {"
                        "  background: %1;"
                        "  color: %2;"
                        "  border: 1px solid %3;"
                        "  border-radius: 8px;"
                        "  font-size: 16px;"
                        "  font-weight: 600;"
                        "}"
                        "QPushButton:hover {"
                        "  background: %4;"
                        "}"
                        "QPushButton:pressed {"
                        "  background: %5;"
                        "}"
                    ).arg(
                        WHITE,
                        TEXT,
                        BORDER,
                        "#f0f0f0",
                        "#e9e9e9"
                    );

                minus_button->setStyleSheet(
                    quantity_style
                );

                plus_button->setStyleSheet(
                    quantity_style
                );

                item_row->addWidget(
                    minus_button
                );

                item_row->addWidget(
                    quantity_label
                );

                item_row->addWidget(
                    plus_button
                );

                auto* add_button =
                    make_button(
                        "ADD",
                        GREEN,
                        WHITE
                    );

                add_button->setFixedWidth(
                    78
                );

                add_button->setMinimumHeight(
                    38
                );

                item_row->addWidget(
                    add_button
                );

                connect(
                    minus_button,
                    &QPushButton::clicked,
                    this,
                    [quantity_label]()
                    {
                        bool ok = false;

                        int quantity =
                            quantity_label
                                ->text()
                                .toInt(&ok);

                        if (!ok)
                            return;

                        if (quantity > 1)
                        {
                            --quantity;

                            quantity_label->setText(
                                QString::number(
                                    quantity
                                )
                            );
                        }
                    }
                );

                connect(
                    plus_button,
                    &QPushButton::clicked,
                    this,
                    [quantity_label]()
                    {
                        bool ok = false;

                        int quantity =
                            quantity_label
                                ->text()
                                .toInt(&ok);

                        if (!ok)
                            return;

                        if (quantity < 99)
                        {
                            ++quantity;

                            quantity_label->setText(
                                QString::number(
                                    quantity
                                )
                            );
                        }
                    }
                );

                const int item_id =
                    item.get_id();

                connect(
                    add_button,
                    &QPushButton::clicked,
                    this,
                    [
                        this,
                        zone_name,
                        table_number,
                        item_id,
                        quantity_label
                    ]()
                    {
                        Zone* zone_ptr =
                            restaurant
                                .get_zone_by_name(
                                    zone_name
                                        .toStdString()
                                );

                        if (!zone_ptr)
                            return;

                        Table* table_ptr =
                            zone_ptr
                                ->get_table_by_number(
                                    table_number
                                );

                        if (!table_ptr)
                            return;

                        Order* order_ptr =
                            table_ptr
                                ->get_order();

                        if (!order_ptr)
                            return;

                        const MenuItem* menu_item =
                            restaurant
                                .get_menu()
                                .get_item_by_id(
                                    item_id
                                );

                        if (!menu_item)
                            return;

                        bool ok = false;

                        const int quantity =
                            quantity_label
                                ->text()
                                .toInt(&ok);

                        if (!ok || quantity <= 0)
                            return;

                        OrderItem order_item(
                            *menu_item,
                            quantity
                        );

                        if (
                            order_ptr
                                ->add_item(
                                    order_item
                                )
                        )
                        {
                            showAddItemPage(
                                *zone_ptr,
                                *table_ptr
                            );
                        }
                    }
                );

                menu_items_layout->addWidget(
                    item_card
                );
            }

            menu_items_layout->addSpacing(
                8
            );
        }
    }

    menu_items_layout->addStretch();

    menu_scroll->setWidget(
        menu_container
    );

    menu_layout->addWidget(
        menu_scroll,
        1
    );

    // ========================================================
    // RIGHT — CURRENT ORDER
    // ========================================================

    auto* order_panel =
        new QFrame;

    order_panel->setObjectName(
        "orderPanel"
    );

    order_panel->setMinimumWidth(
        330
    );

    order_panel->setMaximumWidth(
        400
    );

    order_panel->setStyleSheet(
        QString(
            "QFrame#orderPanel {"
            "  background: %1;"
            "  border: 1px solid %2;"
            "  border-radius: 16px;"
            "}"
        ).arg(
            CARD,
            BORDER
        )
    );

    auto* order_layout =
        new QVBoxLayout(
            order_panel
        );

    order_layout->setContentsMargins(
        22,
        20,
        22,
        20
    );

    order_layout->setSpacing(
        14
    );

    auto* order_header =
        new QHBoxLayout;

    auto* order_title =
        new QVBoxLayout;

    order_title->setSpacing(
        2
    );

    order_title->addWidget(
        make_label(
            "CURRENT ORDER",
            12,
            MUTED,
            QFont::Bold
        )
    );

    const Order* current_order =
        table.get_order();

    const int current_item_count =
        current_order
            ? static_cast<int>(
                current_order
                    ->get_items()
                    .size()
            )
            : 0;

    order_title->addWidget(
        make_label(
            QString("%1 items")
                .arg(
                    current_item_count
                ),
            11,
            MUTED
        )
    );

    order_header->addLayout(
        order_title
    );

    order_header->addStretch();

    order_layout->addLayout(
        order_header
    );

    auto* order_scroll =
        new QScrollArea;

    order_scroll->setWidgetResizable(
        true
    );

    order_scroll->setHorizontalScrollBarPolicy(
        Qt::ScrollBarAlwaysOff
    );

    order_scroll->setStyleSheet(
        "QScrollArea {"
        "  border: none;"
        "  background: transparent;"
        "}"
    );

    auto* order_container =
        new QWidget;

    auto* order_items_layout =
        new QVBoxLayout(
            order_container
        );

    order_items_layout->setContentsMargins(
        0,
        0,
        6,
        0
    );

    order_items_layout->setSpacing(
        8
    );

    if (
        current_order &&
        !current_order
            ->get_items()
            .empty()
    )
    {
        for (
            const OrderItem& item :
            current_order->get_items()
        )
        {
            auto* item_card =
                new QFrame;

            item_card->setStyleSheet(
                QString(
                    "QFrame {"
                    "  background: %1;"
                    "  border: 1px solid %2;"
                    "  border-radius: 10px;"
                    "}"
                ).arg(
                    WHITE,
                    BORDER
                )
            );

            auto* item_layout =
                new QVBoxLayout(
                    item_card
                );

            item_layout->setContentsMargins(
                13,
                11,
                13,
                11
            );

            item_layout->setSpacing(
                5
            );

            auto* name_row =
                new QHBoxLayout;

            name_row->addWidget(
                make_label(
                    QString::fromStdString(
                        item.get_name()
                    ),
                    13,
                    TEXT,
                    QFont::DemiBold
                ),
                1
            );

            name_row->addWidget(
                make_label(
                    QString(
                        "x%1"
                    ).arg(
                        item.get_quantity()
                    ),
                    11,
                    MUTED,
                    QFont::Bold
                )
            );

            item_layout->addLayout(
                name_row
            );

            auto* price_row =
                new QHBoxLayout;

            price_row->addWidget(
                make_label(
                    money(
                        item.get_unit_price()
                    ) + " each",
                    10,
                    MUTED
                )
            );

            price_row->addStretch();

            price_row->addWidget(
                make_label(
                    money(
                        item.get_subtotal()
                    ),
                    12,
                    TEXT,
                    QFont::Bold
                )
            );

            item_layout->addLayout(
                price_row
            );

            order_items_layout->addWidget(
                item_card
            );
        }
    }
    else
    {
        auto* empty_card =
            new QFrame;

        empty_card->setStyleSheet(
            QString(
                "QFrame {"
                "  background: %1;"
                "  border: 1px dashed %2;"
                "  border-radius: 12px;"
                "}"
            ).arg(
                WHITE,
                BORDER
            )
        );

        auto* empty_layout =
            new QVBoxLayout(
                empty_card
            );

        empty_layout->setContentsMargins(
            16,
            28,
            16,
            28
        );

        empty_layout->addWidget(
            make_label(
                "Order is empty",
                14,
                TEXT,
                QFont::DemiBold
            )
        );

        auto* hint =
            make_label(
                "Select an item from the menu.",
                11,
                MUTED
            );

        hint->setWordWrap(
            true
        );

        empty_layout->addWidget(
            hint
        );

        order_items_layout->addWidget(
            empty_card
        );
    }

    order_items_layout->addStretch();

    order_scroll->setWidget(
        order_container
    );

    order_layout->addWidget(
        order_scroll,
        1
    );

    auto* total_separator =
        new QFrame;

    total_separator->setFrameShape(
        QFrame::HLine
    );

    total_separator->setFrameShadow(
        QFrame::Plain
    );

    total_separator->setStyleSheet(
        QString(
            "QFrame {"
            "  color: %1;"
            "  background: %1;"
            "  max-height: 1px;"
            "}"
        ).arg(BORDER)
    );

    order_layout->addWidget(
        total_separator
    );

    const int current_total =
        current_order
            ? current_order->get_total()
            : 0;

    auto* total_row =
        new QHBoxLayout;

    total_row->addWidget(
        make_label(
            "TOTAL",
            11,
            MUTED,
            QFont::Bold
        )
    );

    total_row->addStretch();

    total_row->addWidget(
        make_label(
            money(current_total),
            22,
            TEXT,
            QFont::Bold
        )
    );

    order_layout->addLayout(
        total_row
    );

    main->addWidget(
        menu_panel,
        3
    );

    main->addWidget(
        order_panel,
        1
    );

    outer->addLayout(
        main,
        1
    );

    return page;
}

QWidget* MainWindow::createPaymentPage(
    Zone& zone,
    Table& table)
{
    auto* page =
        new QWidget;

    auto* outer =
        new QVBoxLayout(page);

    outer->setContentsMargins(
        42,
        32,
        42,
        32
    );

    outer->setSpacing(0);

    const QString zone_name =
        QString::fromStdString(
            zone.get_name()
        );

    const int table_number =
        table.get_table_number();

    // ========================================================
    // HEADER
    // ========================================================

    auto* header =
        new QHBoxLayout;

    auto* back_button =
        make_button(
            "Back",
            WHITE,
            TEXT,
            BORDER
        );

    back_button->setFixedWidth(
        86
    );

    back_button->setMinimumHeight(
        38
    );

    connect(
        back_button,
        &QPushButton::clicked,
        this,
        [
            this,
            zone_name,
            table_number
        ]()
        {
            Zone* zone_ptr =
                restaurant
                    .get_zone_by_name(
                        zone_name.toStdString()
                    );

            if (!zone_ptr)
                return;

            Table* table_ptr =
                zone_ptr
                    ->get_table_by_number(
                        table_number
                    );

            if (!table_ptr)
                return;

            showTablePage(
                *zone_ptr,
                *table_ptr
            );
        }
    );

    header->addWidget(
        back_button
    );

    header->addSpacing(
        16
    );

    auto* title =
        new QVBoxLayout;

    title->setSpacing(
        2
    );

    title->addWidget(
        make_label(
            "Payment",
            26,
            TEXT,
            QFont::Bold
        )
    );

    title->addWidget(
        make_label(
            QString("Table %1 · %2")
                .arg(
                    table_number,
                    2,
                    10,
                    QChar('0')
                )
                .arg(
                    zone_name
                ),
            12,
            MUTED
        )
    );

    header->addLayout(
        title
    );

    header->addStretch();

    header->addWidget(
        make_status_badge(
            table.get_status()
        )
    );

    outer->addLayout(
        header
    );

    outer->addSpacing(
        28
    );

    // ========================================================
    // PAYMENT CARD
    // ========================================================

    auto* payment_card =
        make_card();

    payment_card->setMaximumWidth(
        620
    );

    auto* payment_layout =
        new QVBoxLayout(
            payment_card
        );

    payment_layout->setContentsMargins(
        32,
        30,
        32,
        30
    );

    payment_layout->setSpacing(
        18
    );

    // ========================================================
    // TITLE
    // ========================================================

    payment_layout->addWidget(
        make_label(
            "PAYMENT",
            11,
            MUTED,
            QFont::Bold
        )
    );

    payment_layout->addWidget(
        make_label(
            "Select payment method",
            20,
            TEXT,
            QFont::Bold
        )
    );

    // ========================================================
    // TOTAL
    // ========================================================

    Order* order =
        table.get_order();

    const int total =
        order
            ? order->get_total()
            : 0;

    auto* total_card =
        new QFrame;

    total_card->setStyleSheet(
        QString(
            "QFrame {"
            "  background: %1;"
            "  border: 1px solid %2;"
            "  border-radius: 14px;"
            "}"
        ).arg(
            WHITE,
            BORDER
        )
    );

    auto* total_layout =
        new QVBoxLayout(
            total_card
        );

    total_layout->setContentsMargins(
        20,
        18,
        20,
        18
    );

    total_layout->setSpacing(
        4
    );

    total_layout->addWidget(
        make_label(
            "TOTAL DUE",
            10,
            MUTED,
            QFont::Bold
        )
    );

    total_layout->addWidget(
        make_label(
            money(total),
            30,
            TEXT,
            QFont::Bold
        )
    );

    payment_layout->addWidget(
        total_card
    );

    // ========================================================
    // PAYMENT METHOD
    // ========================================================

    payment_layout->addWidget(
        make_label(
            "PAYMENT METHOD",
            10,
            MUTED,
            QFont::Bold
        )
    );

    auto* method_layout =
        new QHBoxLayout;

    method_layout->setSpacing(
        10
    );

    auto* cash_button =
        make_button(
            "CASH",
            WHITE,
            TEXT,
            BORDER
        );

    auto* card_button =
        make_button(
            "CARD",
            WHITE,
            TEXT,
            BORDER
        );

    cash_button->setMinimumHeight(
        52
    );

    card_button->setMinimumHeight(
        52
    );

    cash_button->setCheckable(
        true
    );

    card_button->setCheckable(
        true
    );

    method_layout->addWidget(
        cash_button,
        1
    );

    method_layout->addWidget(
        card_button,
        1
    );

    payment_layout->addLayout(
        method_layout
    );

    // ========================================================
    // SELECTED METHOD
    // ========================================================

    auto* selected_method =
        make_label(
            "No payment method selected",
            11,
            MUTED
        );

    selected_method->setAlignment(
        Qt::AlignCenter
    );

    payment_layout->addWidget(
        selected_method
    );

    // ========================================================
    // METHOD SELECTION
    // ========================================================

    auto select_method =
        [
            cash_button,
            card_button,
            selected_method
        ](
            PaymentMethod method
        )
        {
            const QString selected_style =
                QString(
                    "QPushButton {"
                    "  background: %1;"
                    "  color: %2;"
                    "  border: 2px solid %3;"
                    "  border-radius: 10px;"
                    "  font-weight: 700;"
                    "}"
                ).arg(
                    GREEN_LIGHT,
                    GREEN,
                    GREEN
                );

            const QString normal_style =
                QString(
                    "QPushButton {"
                    "  background: %1;"
                    "  color: %2;"
                    "  border: 1px solid %3;"
                    "  border-radius: 10px;"
                    "  font-weight: 600;"
                    "}"
                    "QPushButton:hover {"
                    "  background: %4;"
                    "}"
                ).arg(
                    WHITE,
                    TEXT,
                    BORDER,
                    "#f0f0f0"
                );

            cash_button->setStyleSheet(
                method == PaymentMethod::Cash
                    ? selected_style
                    : normal_style
            );

            card_button->setStyleSheet(
                method == PaymentMethod::Card
                    ? selected_style
                    : normal_style
            );

            cash_button->setChecked(
                method == PaymentMethod::Cash
            );

            card_button->setChecked(
                method == PaymentMethod::Card
            );

            if (
                method ==
                PaymentMethod::Cash
            )
            {
                selected_method->setText(
                    "Cash payment selected"
                );
            }
            else
            {
                selected_method->setText(
                    "Card payment selected"
                );
            }

            selected_method->setStyleSheet(
                QString(
                    "QLabel {"
                    "  color: %1;"
                    "  font-weight: 600;"
                    "}"
                ).arg(GREEN)
            );
        };

    connect(
        cash_button,
        &QPushButton::clicked,
        this,
        [
            select_method
        ]()
        {
            select_method(
                PaymentMethod::Cash
            );
        }
    );

    connect(
        card_button,
        &QPushButton::clicked,
        this,
        [
            select_method
        ]()
        {
            select_method(
                PaymentMethod::Card
            );
        }
    );

    // ========================================================
    // PROCESSING UI
    // ========================================================

    auto* processing_label =
        make_label(
            "",
            12,
            MUTED,
            QFont::DemiBold
        );

    processing_label->setAlignment(
        Qt::AlignCenter
    );

    processing_label->setVisible(
        false
    );

    payment_layout->addWidget(
        processing_label
    );

    auto* spinner_label =
        make_label(
            "",
            28,
            GREEN,
            QFont::Bold
        );

    spinner_label->setAlignment(
        Qt::AlignCenter
    );

    spinner_label->setVisible(
        false
    );

    payment_layout->addWidget(
        spinner_label
    );

    // ========================================================
    // SEPARATOR
    // ========================================================

    auto* separator =
        new QFrame;

    separator->setFrameShape(
        QFrame::HLine
    );

    separator->setFrameShadow(
        QFrame::Plain
    );

    separator->setStyleSheet(
        QString(
            "QFrame {"
            "  color: %1;"
            "  background: %1;"
            "  max-height: 1px;"
            "}"
        ).arg(BORDER)
    );

    payment_layout->addWidget(
        separator
    );

    // ========================================================
    // CONFIRM BUTTON
    // ========================================================

    auto* confirm_button =
        make_button(
            "CONFIRM PAYMENT",
            GREEN,
            WHITE
        );

    confirm_button->setMinimumHeight(
        48
    );

    payment_layout->addWidget(
        confirm_button
    );

    // ========================================================
    // CARD TERMINAL
    // ========================================================

    auto terminal =
        std::make_shared<CardTerminal>();

    auto spinner_index =
        std::make_shared<int>(0);

    const QStringList spinner_frames = {
        "◐",
        "◓",
        "◑",
        "◒"
    };

    auto* spinner_timer =
        new QTimer(page);

    spinner_timer->setInterval(
        180
    );

    connect(
        spinner_timer,
        &QTimer::timeout,
        page,
        [
            spinner_label,
            spinner_index,
            spinner_frames
        ]()
        {
            if (spinner_frames.isEmpty())
                return;

            spinner_label->setText(
                spinner_frames.at(
                    *spinner_index
                )
            );

            *spinner_index =
                (
                    *spinner_index + 1
                ) %
                spinner_frames.size();
        }
    );

    // ========================================================
    // CONFIRM PAYMENT
    // ========================================================

    connect(
        confirm_button,
        &QPushButton::clicked,
        this,
        [
            this,
            page,
            zone_name,
            table_number,
            cash_button,
            card_button,
            selected_method,
            processing_label,
            spinner_label,
            spinner_timer,
            confirm_button,
            terminal,
            spinner_index

        ]()
        {
            PaymentMethod selected_method_value =
                PaymentMethod::None;

            if (
                cash_button->isChecked()
            )
            {
                selected_method_value =
                    PaymentMethod::Cash;
            }
            else if (
                card_button->isChecked()
            )
            {
                selected_method_value =
                    PaymentMethod::Card;
            }

            // ------------------------------------------------
            // No method selected
            // ------------------------------------------------

            if (
                selected_method_value ==
                PaymentMethod::None
            )
            {
                selected_method->setText(
                    "Please select a payment method"
                );

                selected_method->setStyleSheet(
                    QString(
                        "QLabel {"
                        "  color: %1;"
                        "  font-weight: 600;"
                        "}"
                    ).arg(RED)
                );

                return;
            }

            Zone* zone_ptr =
                restaurant
                    .get_zone_by_name(
                        zone_name.toStdString()
                    );

            if (!zone_ptr)
                return;

            Table* table_ptr =
                zone_ptr
                    ->get_table_by_number(
                        table_number
                    );

            if (!table_ptr)
                return;

            Order* order_ptr =
                table_ptr->get_order();

            if (!order_ptr)
                return;

            // =================================================
            // CASH PAYMENT
            // =================================================

            if (
                selected_method_value ==
                PaymentMethod::Cash
            )
            {
                if (
                    order_ptr->pay_for_order(
                        PaymentMethod::Cash
                    )
                )
                {
                    showTablePage(
                        *zone_ptr,
                        *table_ptr
                    );
                }

                return;
            }

            // =================================================
            // CARD PAYMENT
            // =================================================

            if (
                !order_ptr->start_payment(
                    PaymentMethod::Card
                )
            )
            {
                return;
            }

            if (
                !terminal->start_payment()
            )
            {
                return;
            }

            confirm_button->setEnabled(
                false
            );

            cash_button->setEnabled(
                false
            );

            card_button->setEnabled(
                false
            );

            processing_label->setVisible(
                true
            );

            spinner_label->setVisible(
                true
            );

            processing_label->setText(
                "Connecting to card terminal..."
            );

            processing_label->setStyleSheet(
                QString(
                    "QLabel {"
                    "  color: %1;"
                    "  font-weight: 600;"
                    "}"
                ).arg(MUTED)
            );

            spinner_label->setStyleSheet(
                QString(
                    "QLabel {"
                    "  color: %1;"
                    "  font-weight: 700;"
                    "}"
                ).arg(GREEN)
            );

            *spinner_index = 0;

            spinner_timer->start();

            // 2–4 seconds

            const int delay =
                QRandomGenerator::global()
                    ->bounded(
                        2000,
                        4001
                    );

            QTimer::singleShot(
                delay,
                page,
                [
                    this,
                    zone_name,
                    table_number,
                    order_ptr,
                    processing_label,
                    spinner_label,
                    spinner_timer,
                    confirm_button,
                    cash_button,
                    card_button,
                    terminal
                ]()
                {
                    spinner_timer->stop();

                    terminal->process_payment();

                    const CardTerminalStatus status =
                        terminal->get_status();

                    // =========================================
                    // APPROVED
                    // =========================================

                    if (
                        status ==
                        CardTerminalStatus::Approved
                    )
                    {
                        if (
                            order_ptr
                                ->approve_payment()
                        )
                        {
                            processing_label->setText(
                                "PAYMENT APPROVED"
                            );

                            processing_label->setStyleSheet(
                                QString(
                                    "QLabel {"
                                    "  color: %1;"
                                    "  font-weight: 700;"
                                    "}"
                                ).arg(GREEN)
                            );

                            spinner_label->setText(
                                "✓"
                            );

                            spinner_label->setStyleSheet(
                                QString(
                                    "QLabel {"
                                    "  color: %1;"
                                    "  font-weight: 700;"
                                    "}"
                                ).arg(GREEN)
                            );

                            QTimer::singleShot(
                                700,
                                this,
                                [
                                    this,
                                    zone_name,
                                    table_number
                                ]()
                                {
                                    Zone* zone_ptr =
                                        restaurant
                                            .get_zone_by_name(
                                                zone_name.toStdString()
                                            );

                                    if (!zone_ptr)
                                        return;

                                    Table* table_ptr =
                                        zone_ptr
                                            ->get_table_by_number(
                                                table_number
                                            );

                                    if (!table_ptr)
                                        return;

                                    showTablePage(
                                        *zone_ptr,
                                        *table_ptr
                                    );
                                }
                            );

                            return;
                        }
                    }

                    // =========================================
                    // DECLINED
                    // =========================================

                    if (
                        status ==
                        CardTerminalStatus::Declined
                    )
                    {
                        order_ptr
                            ->decline_payment();

                        processing_label->setText(
                            "PAYMENT DECLINED"
                        );

                        processing_label->setStyleSheet(
                            QString(
                                "QLabel {"
                                "  color: %1;"
                                "  font-weight: 700;"
                                "}"
                            ).arg(RED)
                        );

                        spinner_label->setText(
                            "✕"
                        );

                        spinner_label->setStyleSheet(
                            QString(
                                "QLabel {"
                                "  color: %1;"
                                "  font-weight: 700;"
                                "}"
                            ).arg(RED)
                        );

                        confirm_button->setText(
                            "TRY AGAIN"
                        );

                        confirm_button->setEnabled(
                            true
                        );

                        cash_button->setEnabled(
                            true
                        );

                        card_button->setEnabled(
                            true
                        );

                        return;
                    }

                    // =========================================
                    // CONNECTION ERROR
                    // =========================================

                    if (
                        status ==
                        CardTerminalStatus::ConnectionError
                    )
                    {
                        order_ptr
                            ->cancel_payment();

                        processing_label->setText(
                            "CONNECTION ERROR"
                        );

                        processing_label->setStyleSheet(
                            QString(
                                "QLabel {"
                                "  color: %1;"
                                "  font-weight: 700;"
                                "}"
                            ).arg(ORANGE)
                        );

                        spinner_label->setText(
                            "!"
                        );

                        spinner_label->setStyleSheet(
                            QString(
                                "QLabel {"
                                "  color: %1;"
                                "  font-weight: 700;"
                                "}"
                            ).arg(ORANGE)
                        );

                        confirm_button->setText(
                            "TRY AGAIN"
                        );

                        confirm_button->setEnabled(
                            true
                        );

                        cash_button->setEnabled(
                            true
                        );

                        card_button->setEnabled(
                            true
                        );

                        return;
                    }

                    confirm_button->setEnabled(
                        true
                    );

                    cash_button->setEnabled(
                        true
                    );

                    card_button->setEnabled(
                        true
                    );
                }
            );
        }
    );

    payment_layout->addStretch();

    outer->addWidget(
        payment_card,
        0,
        Qt::AlignHCenter
    );

    outer->addStretch();

    return page;
}

// ============================================================
// Payment Page Navigation
// ============================================================

void MainWindow::showPaymentPage(
    Zone& zone,
    Table& table)
{
    while (content->count() > 0)
    {
        QWidget* old =
            content->widget(0);

        content->removeWidget(
            old
        );

        old->deleteLater();
    }

    QWidget* page =
        createPaymentPage(
            zone,
            table
        );

    content->addWidget(
        page
    );

    content->setCurrentWidget(
        page
    );
}


// ============================================================
// Navigation helpers
// ============================================================

void MainWindow::showZonesPage()
{
    while (content->count() > 0)
    {
        QWidget* old =
            content->widget(0);

        content->removeWidget(
            old
        );

        old->deleteLater();
    }

    QWidget* page =
        createZonesPage();

    content->addWidget(
        page
    );

    content->setCurrentWidget(
        page
    );
}


void MainWindow::showTablesPage(
    Zone& zone)
{
    while (content->count() > 0)
    {
        QWidget* old =
            content->widget(0);

        content->removeWidget(
            old
        );

        old->deleteLater();
    }

    QWidget* page =
        createTablesPage(
            zone
        );

    content->addWidget(
        page
    );

    content->setCurrentWidget(
        page
    );
}


void MainWindow::showTablePage(
    Zone& zone,
    Table& table)
{
    while (content->count() > 0)
    {
        QWidget* old =
            content->widget(0);

        content->removeWidget(
            old
        );

        old->deleteLater();
    }

    QWidget* page =
        createTablePage(
            zone,
            table
        );

    content->addWidget(
        page
    );

    content->setCurrentWidget(
        page
    );
}


void MainWindow::showAddItemPage(
    Zone& zone,
    Table& table)
{
    while (content->count() > 0)
    {
        QWidget* old =
            content->widget(0);

        content->removeWidget(
            old
        );

        old->deleteLater();
    }

    QWidget* page =
        createAddItemPage(
            zone,
            table
        );

    content->addWidget(
        page
    );

    content->setCurrentWidget(
        page
    );
}