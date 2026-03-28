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

protected:
    void paintEvent(QPaintEvent *) override;
    void keyPressEvent(QKeyEvent *) override;
    void keyReleaseEvent(QKeyEvent *e) override; // 必须和实现一样
private slots:
    void updateGame();

private:
    void initGame();
    void createPipe();
    bool checkCollision();

    QTimer *timer;

    // 小鸟
    int birdY;
    int birdVelocity;
    const int birdX = 80;
    const int birdSize = 30;

    // 管道
    struct Pipe {
        int x;
        int gapY;
    };
    QList<Pipe> pipes;

    bool gameOver;
    int score;
    int speed;

    bool upPress, downPress; // 加这一行
};

#endif // WIDGET_H