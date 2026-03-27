#ifndef WIDGET_H
#define WIDGET_H

#include <QWidget>
#include <QTimer>
#include <QList>
#include <QPoint>
#include <QKeyEvent>  // 加上这一行！

class Widget : public QWidget
{
    Q_OBJECT

public:
    Widget(QWidget *parent = nullptr);
    ~Widget();

protected:
    void paintEvent(QPaintEvent *event) override;
    void keyPressEvent(QKeyEvent *event) override;  // 现在编译器认识了

private slots:
    void moveSnake();

private:
    void initGame();
    void createFood();
    void checkCollision();

private:
    QTimer *timer;
    QList<QPoint> snake;
    QPoint food;
    int dir;
    bool gameOver;
    int score;

    bool isPaused;    // 暂停标记
    int speedLevel;   // 速度等级 1~3
    void setSpeed(int level); // 设置速度
};

#endif // WIDGET_H