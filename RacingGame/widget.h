#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QPainter>
#include <QKeyEvent>
#include <QList>

class Widget : public QWidget
{
    Q_OBJECT
public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void paintEvent(QPaintEvent *) override;
    void keyPressEvent(QKeyEvent *) override;
    void keyReleaseEvent(QKeyEvent *) override;

private slots:
    void gameUpdate();

private:
    void initGame();
    void spawnObstacle();
    bool checkCollision();

    QTimer* timer;

    // 道路滚动
    int lineY1, lineY2;

    // 玩家
    int playerX;
    int playerW = 50;
    int playerH = 90;

    // 障碍物
    struct Obstacle {
        int x, y, w, h;
    };
    QList<Obstacle> obstacles;

    bool left, right;
    bool gameOver;
    int score;
    int speed;
};

#endif // WIDGET_H