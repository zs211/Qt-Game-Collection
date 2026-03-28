#include "widget.h"
#include <QPainter>
#include <QRandomGenerator>
#include <QFont>
#include <QPen>

Widget::Widget(QWidget *parent)
    : QWidget(parent)
{
    setFixedSize(GAME_WIDTH, GAME_HEIGHT);
    initGame();
}

Widget::~Widget()
{
    delete timer;
}

void Widget::initGame()
{
    planePos = QPoint(GAME_WIDTH/2 - 20, GAME_HEIGHT - 80);
    bullets.clear();
    enemies.clear();
    keys.clear();
    isGameOver = false;
    score = 0;


    difficulty = 1; // 必须初始化！
    enemyInterval = 80; // 对应难度1的间隔

    timer = new QTimer(this);
    connect(timer, &QTimer::timeout, this, &Widget::gameLoop);
    timer->start(16); // ~60 FPS

    setDifficulty(difficulty); // 初始化难度

    planePix.load(":/prefix1/plane.png");
    enemyPix.load(":/prefix1/enemy.jpeg");
}

void Widget::gameLoop()
{
    if(isGameOver) return;

    // 移动飞机
    if(keys.contains(Qt::Key_Up) && planePos.y() > 0)
        planePos.ry() -= PLANE_SPEED;
    if(keys.contains(Qt::Key_Down) && planePos.y() < GAME_HEIGHT - 60)
        planePos.ry() += PLANE_SPEED;
    if(keys.contains(Qt::Key_Left) && planePos.x() > 0)
        planePos.rx() -= PLANE_SPEED;
    if(keys.contains(Qt::Key_Right) && planePos.x() < GAME_WIDTH - 40)
        planePos.rx() += PLANE_SPEED;

    // 发射子弹（空格）
    if(keys.contains(Qt::Key_Space)){
        static int lastShot = 0;
        if(lastShot++ % 8 == 0){ // 控制射速
            bullets << QPoint(planePos.x() + 18, planePos.y() - 10);
        }
    }

    moveBullets();
    if(QRandomGenerator::global()->bounded(50) == 0) createEnemy(); // 随机生成敌机
    moveEnemies();
    checkCollisions();
    // 更新爆炸动画
    QList<Explosion> newExplosions;
    for(Explosion &exp : explosions){
        exp.frame++;
        if(exp.frame < 6)  // 动画持续6帧
            newExplosions << exp;
    }
    explosions = newExplosions;

    update();
}

void Widget::moveBullets()
{
    QList<QPoint> newBullets;
    for(const QPoint &b : bullets){
        QPoint newB = b - QPoint(0, BULLET_SPEED);
        if(newB.y() > -20) newBullets << newB;
    }
    bullets = newBullets;
}

void Widget::createEnemy()
{
    int x = QRandomGenerator::global()->bounded(GAME_WIDTH - 40);
    enemies << QPoint(x, -40);
}

void Widget::moveEnemies()
{
    QList<QPoint> newEnemies;
    for(const QPoint &e : enemies){
        QPoint newE = e + QPoint(0, ENEMY_SPEED);
        if(newE.y() < GAME_HEIGHT) newEnemies << newE;
    }
    enemies = newEnemies;
}

void Widget::checkCollisions()
{
    QList<QPoint> newBullets;
    QList<QPoint> newEnemies;

    // 遍历所有敌机，判断是否被击中
    for(const QPoint &e : enemies)
    {
        bool isHit = false;
        // 遍历所有子弹，看是否击中这架敌机
        for(const QPoint &b : bullets)
        {
            if(abs(b.x() - e.x()) < 30 && abs(b.y() - e.y()) < 30)
            {
                isHit = true;
                score += 10;
                explosions << Explosion{e, 0}; // 生成爆炸
                break;
            }
        }
        // 没被击中的敌机才保留
        if(!isHit)
            newEnemies << e;
        // 被击中的子弹也要移除（避免重复击中）
        if(isHit)
        {
            QList<QPoint> tempBullets;
            for(const QPoint &b : bullets)
            {
                if(!(abs(b.x() - e.x()) < 30 && abs(b.y() - e.y()) < 30))
                    tempBullets << b;
            }
            bullets = tempBullets;
        }
    }
    enemies = newEnemies;

    // 敌机 vs 玩家（修复后的碰撞范围）
    for(const QPoint &e : enemies)
    {
        if(abs(e.x() - planePos.x()) < 25 && abs(e.y() - planePos.y()) < 35)
        {
            gameOver();
            return;
        }
    }
}

void Widget::gameOver()
{
    isGameOver = true;
    timer->stop();
    update();
}

void Widget::paintEvent(QPaintEvent *)
{
    QPainter p(this);
    p.fillRect(rect(), QColor(20,20,40)); // 深蓝背景
    // 调试：打印加载状态
    qDebug() << "planePix isNull:" << planePix.isNull();
    qDebug() << "planePix size:" << planePix.size();
    qDebug() << "enemyPix isNull:" << enemyPix.isNull();
    // 画玩家飞机（用图片）
    // 缩放为 40x60 像素（适合玩家飞机）
    p.drawPixmap(planePos, planePix.scaled(40, 60, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // 画子弹（黄色小圆点）
    p.setBrush(Qt::yellow);
    for(const QPoint &b : bullets)
        p.drawEllipse(b, 3, 3);

    // 画敌机（用图片）
    for(const QPoint &e : enemies)
    // 缩放为 40x40 像素（适合敌机）
    p.drawPixmap(e, enemyPix.scaled(40, 40, Qt::KeepAspectRatio, Qt::SmoothTransformation));

    // 画分数
    p.setPen(Qt::white);
    p.setFont(QFont("Arial", 16));
    p.drawText(10, 30, QString("Score: %1").arg(score));


    // 画爆炸效果
    for(const Explosion &exp : explosions){
        if(!explosionPix.isNull()){
            // 随帧放大，模拟爆炸扩散
            int size = 20 + exp.frame * 5;
            p.drawPixmap(
                exp.pos.x() - size/2, exp.pos.y() - size/2,
                explosionPix.scaled(size, size, Qt::KeepAspectRatio, Qt::SmoothTransformation)
                );
        }
    }

    // 游戏结束
    if(isGameOver){
        p.setPen(Qt::red);
        p.setFont(QFont("Arial", 30, QFont::Bold));
        p.drawText(rect(), Qt::AlignCenter, "GAME OVER\nScore: " + QString::number(score));
    }
}

void Widget::keyPressEvent(QKeyEvent *e)
{
    Qt::Key key = static_cast<Qt::Key>(e->key());
    if(!keys.contains(key))
        keys << key;

    if(isGameOver){
        if(key == Qt::Key_Space)
            initGame();
        return;
    }

    switch(key){
    case Qt::Key_1: setDifficulty(1); break;
    case Qt::Key_2: setDifficulty(2); break;
    case Qt::Key_3: setDifficulty(3); break;
    }
}

void Widget::keyReleaseEvent(QKeyEvent *e)
{
    Qt::Key key = static_cast<Qt::Key>(e->key());
    keys.removeOne(key);
}

void Widget::setDifficulty(int level)
{
    int enemyInterval;
    switch(level){
    case 1: enemyInterval = 80; break;
    case 2: enemyInterval = 40; break;
    case 3: enemyInterval = 20; break;
    default: enemyInterval = 80;
    }
    // 调整敌机生成频率
    this->enemyInterval = enemyInterval; // 记得在头文件里加这个变量
    difficulty = level;
    setWindowTitle(QString("飞机大战 | Score: %1 | 难度: %2").arg(score).arg(level));
}