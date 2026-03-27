#include "widget.h"
#include <QPainter>
#include <QRandomGenerator>

#define BLOCK 20
#define W 40
#define H 30

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(W*BLOCK, H*BLOCK);
    initGame();
}

Widget::~Widget()
{
    delete timer;
}

void Widget::initGame()
{
    snake.clear();
    snake << QPoint(5,5) << QPoint(5,6) << QPoint(5,7);
    dir = 4;
    gameOver = false;
    score = 0;
    setWindowTitle("贪吃蛇 Score:0");


    isPaused = false;

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Widget::moveSnake);
    timer->start(120);
    setSpeed(speedLevel); // 初始化速度
    createFood();
}

void Widget::createFood()
{
    int x = QRandomGenerator::global()->bounded(W);
    int y = QRandomGenerator::global()->bounded(H);
    food = QPoint(x,y);
}

void Widget::moveSnake()
{
    if(gameOver) return;

    QPoint head = snake.first();
    switch(dir){
    case 1: head.setY(head.y() - 1); break;  // 上：y减1
    case 2: head.setY(head.y() + 1); break;  // 下：y加1
    case 3: head.setX(head.x() - 1); break;  // 左：x减1
    case 4: head.setX(head.x() + 1); break;  // 右：x加1
    }

    snake.insert(0, head);

    if(head == food){
        score += 10;
        setWindowTitle(QString("贪吃蛇 Score:%1").arg(score));
        createFood();
    }else{
        snake.removeLast();
    }

    checkCollision();
    update();
}

void Widget::checkCollision()
{
    QPoint head = snake.first();
    if(head.x()<0||head.x()>=W||head.y()<0||head.y()>=H) gameOver=true;
    for(int i=1;i<snake.size();i++) if(snake[i]==head) gameOver=true;
    if(gameOver) timer->stop();
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(30,30,30)); // 深灰背景

    // 画蛇：渐变绿色
    p.setPen(Qt::NoPen);
    for(int i=0; i<snake.size(); i++){
        QPoint pt = snake[i];
        int alpha = 255 - i*3; // 尾巴更淡
        p.setBrush(QColor(0, 200, 50, alpha));
        p.drawRoundedRect(pt.x()*BLOCK+2, pt.y()*BLOCK+2, BLOCK-4, BLOCK-4, 3, 3);
    }

    // 画食物：发光红色
    p.setPen(QPen(QColor(255,100,100), 2));
    p.setBrush(QColor(255,50,50));
    p.drawEllipse(food.x()*BLOCK+2, food.y()*BLOCK+2, BLOCK-4, BLOCK-4);

    if(gameOver){
        p.setPen(Qt::red);
        p.setFont(QFont("Arial", 30, QFont::Bold));
        p.drawText(rect(), Qt::AlignCenter, "GAME OVER\n按空格重新开始");
    }
    if(isPaused && !gameOver){
        p.setPen(Qt::yellow);
        p.setFont(QFont("Arial", 24));
        p.drawText(rect(), Qt::AlignCenter, "PAUSED\n按空格继续");
    }
}

void Widget::keyPressEvent(QKeyEvent *e)
{
    // 游戏结束 → 只能按空格重启
    if(gameOver)
    {
        if(e->key() == Qt::Key_Space)
            initGame();
        return;
    }

    // 游戏正在运行 → 处理方向键 / 暂停 / 难度
    switch(e->key())
    {
    case Qt::Key_Up:
        if(dir != 2) dir = 1;
        break;
    case Qt::Key_Down:
        if(dir != 1) dir = 2;
        break;
    case Qt::Key_Left:
        if(dir != 4) dir = 3;
        break;
    case Qt::Key_Right:
        if(dir != 3) dir = 4;
        break;
    case Qt::Key_Space: // 【修复在这里】
        isPaused = !isPaused;
        if(isPaused)
            timer->stop();
        else
            timer->start(); // 继续游戏
        break;
    case Qt::Key_1:
        setSpeed(1);
        break;
    case Qt::Key_2:
        setSpeed(2);
        break;
    case Qt::Key_3:
        setSpeed(3);
        break;
    }
}

void Widget::setSpeed(int level)
{
    int interval;
    switch(level){
    case 1: interval = 150; break;
    case 2: interval = 100; break;
    case 3: interval = 60;  break;
    default: interval = 150;
    }
    timer->setInterval(interval);
    speedLevel = level;
    setWindowTitle(QString("贪吃蛇 Score:%1 | 速度:%2").arg(score).arg(level));
}