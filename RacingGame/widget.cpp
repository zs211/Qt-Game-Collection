#include "widget.h"

Widget::Widget(QWidget *parent)
    : QWidget{parent}
{
    setFixedSize(420, 700);
    initGame();

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Widget::gameUpdate);
    timer->start(16);
}

Widget::~Widget()
{
    delete timer;
}

void Widget::initGame()
{
    left = false;
    right = false;
    gameOver = false;
    score = 0;
    speed = 6;

    playerX = width()/2 - playerW/2;
    lineY1 = 0;
    lineY2 = height()/2;
    obstacles.clear();
}

void Widget::spawnObstacle()
{
    if (rand() % 45 == 0) {
        int w = 50;
        int h = 90;
        int x = (rand() % 3) * (width()/3) + (width()/6 - w/2);
        obstacles.append({x, -h, w, h});
    }
}

void Widget::gameUpdate()
{
    if (gameOver) return;

    // 车道线滚动
    lineY1 += speed;
    lineY2 += speed;
    if (lineY1 > height()) lineY1 = -height()/2;
    if (lineY2 > height()) lineY2 = -height()/2;

    // 玩家移动
    if (left && playerX > 30)
        playerX -= 7;
    if (right && playerX < width() - 30 - playerW)
        playerX += 7;

    // 障碍物移动
    for (int i=0; i<obstacles.size(); i++) {
        obstacles[i].y += speed;
        if (obstacles[i].y > height()) {
            obstacles.removeAt(i);
            score++;
        }
    }

    spawnObstacle();
    if (checkCollision()) {
        gameOver = true;
    }

    update();
}

bool Widget::checkCollision()
{
    for (auto& o : obstacles) {
        int px1 = playerX;
        int px2 = playerX + playerW;
        int py1 = height() - 120;
        int py2 = py1 + playerH;

        int ox1 = o.x;
        int ox2 = o.x + o.w;
        int oy1 = o.y;
        int oy2 = o.y + o.h;

        if (px1 < ox2 && px2 > ox1 && py1 < oy2 && py2 > oy1) {
            return true;
        }
    }
    return false;
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.setRenderHint(QPainter::Antialiasing);

    // 背景渐变（高级感）
    QLinearGradient bg(0,0,0,height());
    bg.setColorAt(0, QColor(30,30,50));
    bg.setColorAt(1, QColor(10,10,20));
    p.fillRect(rect(), bg);

    // 道路
    p.fillRect(70, 0, 280, height(), QColor(40,40,40));

    // 车道线（虚线）
    p.setPen(QPen(Qt::white, 4));
    p.drawLine(210, lineY1, 210, lineY1 + 60);
    p.drawLine(210, lineY2, 210, lineY2 + 60);

    // 路肩
    p.setPen(QPen(QColor(0,200,100), 3));
    p.drawLine(70,0,70,height());
    p.drawLine(350,0,350,height());

    // 玩家车（好看的渐变跑车）
    QRect car(playerX, height() - 120, playerW, playerH);
    QLinearGradient carColor(car.topLeft(), car.bottomRight());
    carColor.setColorAt(0, QColor(0,220,255));
    carColor.setColorAt(1, QColor(0,100,255));
    p.setBrush(carColor);
    p.setPen(Qt::NoPen);
    p.drawRoundedRect(car, 8, 8);

    // 障碍物（敌方车）
    for (auto& o : obstacles) {
        QRect obs(o.x, o.y, o.w, o.h);
        p.setBrush(QColor(255,60,60));
        p.drawRoundedRect(obs, 8, 8);
    }

    // 分数
    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 22, QFont::Bold));
    p.drawText(20, 40, "Score: " + QString::number(score));

    // 游戏结束
    if (gameOver) {
        p.setPen(QColor(255,60,60));
        p.setFont(QFont("Arial", 40, QFont::Bold));
        p.drawText(rect(), Qt::AlignCenter, "GAME OVER");
    }
}

void Widget::keyPressEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Left)  left = true;
    if (e->key() == Qt::Key_Right) right = true;
    if (e->key() == Qt::Key_Space && gameOver) initGame();
}

void Widget::keyReleaseEvent(QKeyEvent *e)
{
    if (e->key() == Qt::Key_Left)  left = false;
    if (e->key() == Qt::Key_Right) right = false;
}