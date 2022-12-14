//    ______
//   /_  __/___  ____ ___  ___  ____
//    / / / __ \/ __ `__ \/ _ \/ __ \
//   / / / /_/ / / / / / /  __/ /_/ /
//  /_/  \____/_/ /_/ /_/\___/\____/
//              video for sports enthusiasts...
//
//

#include <QtWidgets/QPushButton>
#include <QtWidgets/QHBoxLayout>
#include <QtCore/QFileInfo>
#include <QtWidgets/QFileIconProvider>
#include <QDesktopServices>
#include <QImageReader>
#include <QMessageBox>
#include <QtCore/QDir>
#include <iostream>
#include <QApplication>
#include <QtMultimediaWidgets/QVideoWidget>
#include <QMediaPlaylist>
#include <QLCDNumber>
#include <QComboBox>
#include <QIcon>
#include <QScrollArea>
#include <string>
#include <vector>
#include <QtCore/QDirIterator>
#include <QMainWindow>
#include <QMenu>
#include <QMenuBar>
#include <QStatusBar>
#include <QTableWidget>
#include <QTableWidgetItem>
#include <QLabel>
#include "the_player.h"
#include "the_button.h"
// read in videos and thumbnails to this directory
std::vector<TheButtonInfo> getInfoIn (std::string loc) {

    std::vector<TheButtonInfo> out =  std::vector<TheButtonInfo>();
    QDir dir(QString::fromStdString(loc) );
    QDirIterator it(dir);

    while (it.hasNext()) { // for all files

        QString f = it.next();

            if (f.contains("."))

#if defined(_WIN32)
            if (f.contains(".wmv"))  { // windows
#else
            if (f.contains(".mp4") || f.contains("MOV"))  { // mac/linux
#endif

            QString thumb = f.left( f .length() - 4) +".png";
            if (QFile(thumb).exists()) { // if a png thumbnail exists
                QImageReader *imageReader = new QImageReader(thumb);
                    QImage sprite = imageReader->read(); // read the thumbnail
                    if (!sprite.isNull()) {
                        QIcon* ico = new QIcon(QPixmap::fromImage(sprite)); // voodoo to create an icon for the button
                        QUrl* url = new QUrl(QUrl::fromLocalFile( f )); // convert the file location to a generic url
                        out . push_back(TheButtonInfo( url , ico  ) ); // add to the output list
                    }
                    else
                        qDebug() << "warning: skipping video because I couldn't process thumbnail " << thumb << endl;
            }
            else
                qDebug() << "warning: skipping video because I couldn't find thumbnail " << thumb << endl;
        }
    }

    return out;
}


int main(int argc, char *argv[]) {

    // let's just check that Qt is operational first
    qDebug() << "Qt version: " << QT_VERSION_STR << endl;

    // create the Qt Application
    QApplication app(argc, argv);

    // collect all the videos in the folder
    std::vector<TheButtonInfo> videos;

    if (argc == 2)
        videos = getInfoIn( std::string(argv[1]) );

    if (videos.size() == 0) {

        const int result = QMessageBox::information(
                    NULL,
                    QString("Tomeo"),
                    QString("no videos found! Add command line argument to \"quoted\" file location."));
        exit(-1);
    }

    // the widget that will show the video
    QVideoWidget *videoWidget = new QVideoWidget;

    // the QMediaPlayer which controls the playback
    ThePlayer *player = new ThePlayer;
    player->setVideoOutput(videoWidget);

    // a row of buttons
    QWidget *buttonWidget = new QWidget();
    // a list of the buttons
    std::vector<TheButton*> buttons;
    // the buttons are arranged horizontally
    QVBoxLayout *layout = new QVBoxLayout();
    layout->setMargin(0);
    buttonWidget->setLayout(layout);
    // create function buttons widget
    QWidget *function_buttonWidget = new QWidget();
    std::vector<QPushButton*> function_buttons;
    QHBoxLayout *functionButton_layout = new QHBoxLayout();
    functionButton_layout->setMargin(0);
    function_buttonWidget->setLayout(functionButton_layout);
    //播放键
    QPushButton *play = new QPushButton;
    play->setFixedSize(40,40);
    play ->setIconSize(QSize(40, 40));
    play ->setIcon(play->style()->standardIcon(QStyle::SP_MediaPlay));
    play ->connect(play, &QPushButton::clicked, player, &QMediaPlayer::play);
    function_buttons.push_back(play);
    //时间轴
    QWidget *timeline_row = new QWidget();
    QHBoxLayout *timeline_row_layout = new QHBoxLayout();
    timeline_row->setLayout(timeline_row_layout);
    timeline_row_layout ->setMargin(0);
    timeline_row_layout ->setSpacing(0);
    QSlider *timelineScrollbar = new QSlider(Qt::Horizontal);
    timelineScrollbar->setMaximumHeight(15);
    timelineScrollbar->setObjectName("durationSlider");
    timeline_row_layout->addWidget(timelineScrollbar);
    //上一个
    QPushButton *skipBackward = new QPushButton;
    skipBackward->setFixedSize(40,40);
    skipBackward ->setIconSize(QSize(40, 40));
    skipBackward ->setIcon(skipBackward->style()->standardIcon(QStyle::SP_MediaSkipBackward));
    skipBackward ->connect(skipBackward, &QPushButton::clicked, player, &QMediaPlayer::play);   
    function_buttons.push_back(skipBackward);
    //下一个
    QPushButton *skipForward = new QPushButton;
    skipForward->setFixedSize(40,40);
    skipForward->setIconSize(QSize(40, 40));
    skipForward->setIcon(skipForward->style()->standardIcon(QStyle::SP_MediaSkipForward));
    skipForward->connect(skipForward, &QPushButton::clicked, player, &QMediaPlayer::play);
    function_buttons.push_back(skipForward);
    //音量键
    QPushButton *volume = new QPushButton;
    volume ->setFixedSize(40,40);
    volume ->setIconSize(QSize(40, 40));
    volume ->setIcon(volume->style()->standardIcon(QStyle::SP_MediaVolume));
    volume ->connect(volume, &QPushButton::clicked, player, &QMediaPlayer::play);
    function_buttons.push_back(volume);
    //音量轴
     QSlider *volumelineScrollbar = new QSlider(Qt::Horizontal);
     volumelineScrollbar->setMaximumHeight(15);
     volumelineScrollbar->setObjectName("volumeSlider");
     volumelineScrollbar->setRange(0, 100);
     volumelineScrollbar->setMaximumWidth(150);
     volumelineScrollbar->setObjectName("volumeSlider");
     volumelineScrollbar->setValue(50);
    //组件嵌入布局(3个功能按钮为一组)
    QWidget *function_buttons_group1 = new QWidget();
    QHBoxLayout *function_buttons_group1_layout = new QHBoxLayout();
    function_buttons_group1_layout->setSpacing(10);
    function_buttons_group1_layout->setMargin(0);
    function_buttons_group1->setLayout(function_buttons_group1_layout);
    function_buttons_group1_layout->addWidget(play);
    function_buttons_group1_layout->addWidget(skipBackward);
    function_buttons_group1_layout->addWidget(skipForward);
    functionButton_layout->setSpacing(0);
    functionButton_layout->addWidget(function_buttons_group1);
    functionButton_layout->addStretch();
    functionButton_layout->addWidget(volume);
    functionButton_layout->addWidget(volumelineScrollbar);


    //播客信息行
    QWidget *updater_row = new QWidget();
    QHBoxLayout *updater_row_layout = new QHBoxLayout();
    updater_row->setLayout(updater_row_layout);
    updater_row_layout->setMargin(0);
    updater_row_layout->setSpacing(0);
    //头像
    QPushButton *updater_photo = new QPushButton();
    updater_photo ->setIconSize(QSize(40, 40));
    updater_photo->setIcon(QIcon(":/icon/person-circle.svg"));
    updater_photo->setFixedSize(80,80);
    QPushButton *updater_name = new QPushButton;
    updater_name->setText("Jhon Newo");
    QPushButton *thumb = new QPushButton();
    thumb->setIconSize(QSize(40,40));
    thumb->setIcon(QIcon(":/icon/hand-thumbs-up-fill.svg"));
    thumb->setFixedSize(40,40);
    QPushButton *share = new QPushButton();
    share->setIconSize(QSize(40,40));
    share->setIcon(QIcon(":/icon/play.svg"));
    share->setFixedSize(40,40);

    updater_row_layout->addWidget(updater_photo);
    updater_row_layout->addStretch();
    updater_row_layout->addWidget(thumb);
    updater_row_layout->addWidget(share);
    //侧栏视频
    for ( int i = 0; i < 4; i++ ) {
        TheButton *button = new TheButton(buttonWidget);
        button->connect(button, SIGNAL(jumpTo(TheButtonInfo* )), player, SLOT (jumpTo(TheButtonInfo*))); // when clicked, tell the player to play.
        buttons.push_back(button);
        layout->addWidget(button);
        button->init(&videos.at(i));
    }

    // tell the player what buttons and videos are available
    player->setContent(&buttons, & videos);
    // create the main window and layout


    QWidget window;
    QWidget* leftSide = new QWidget();
    QWidget* rightSide = new QWidget();
    QHBoxLayout *main = new QHBoxLayout();
    QVBoxLayout *left_layout = new QVBoxLayout();
    QVBoxLayout *right_layout = new QVBoxLayout();
    main->setMargin(0);
    main->setSpacing(0);
    left_layout ->setSpacing(0);
    left_layout ->setMargin(0);
    right_layout->setSpacing(0);
    right_layout->setMargin(0);

    window.setLayout(main);
    window.setWindowTitle("tomeo");
    window.setMinimumSize(800, 680);
    leftSide->setLayout(left_layout);
    rightSide->setLayout(right_layout);

    // add the video and the buttons to the top level widget
    left_layout ->addWidget(videoWidget);
    left_layout ->addWidget(timeline_row);
    left_layout ->addWidget(function_buttonWidget);
    left_layout ->addWidget(updater_row);
    right_layout->addWidget(buttonWidget);
    main->addWidget(leftSide);
    main->addWidget(rightSide);
    // showtime!
    window.show();

    // wait for the app to terminate
    return app.exec();
}
