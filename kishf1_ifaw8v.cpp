//=============================================================================================
// Szamitogepes grafika hazi feladat keret. Ervenyes 2011-tol.          
// A //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// sorokon beluli reszben celszeru garazdalkodni, mert a tobbit ugyis toroljuk. 
// A beadott program csak ebben a fajlban lehet, a fajl 1 byte-os ASCII karaktereket tartalmazhat. 
// Tilos:
// - mast "beincludolni", illetve mas konyvtarat hasznalni
// - faljmuveleteket vegezni (printf is fajlmuvelet!)
// - new operatort hivni
// - felesleges programsorokat a beadott programban hagyni
// - tovabbi kommenteket a beadott programba irni a forrasmegjelolest kommentjeit kiveve
// ---------------------------------------------------------------------------------------------
// A feladatot ANSI C++ nyelvu forditoprogrammal ellenorizzuk, a Visual Studio-hoz kepesti elteresekrol
// es a leggyakoribb hibakrol (pl. ideiglenes objektumot nem lehet referencia tipusnak ertekul adni)
// a hazibeado portal ad egy osszefoglalot.
// ---------------------------------------------------------------------------------------------
// A feladatmegoldasokban csak olyan gl/glu/glut fuggvenyek hasznalhatok, amelyek
// 1. Az oran a feladatkiadasig elhangzottak ES (logikai AND muvelet)
// 2. Az alabbi listaban szerepelnek:  
// Rendering pass: glBegin, glVertex[2|3]f, glColor3f, glNormal3f, glTexCoord2f, glEnd, glDrawPixels
// Transzformaciok: glViewport, glMatrixMode, glLoadIdentity, glMultMatrixf, gluOrtho2D, 
// glTranslatef, glRotatef, glScalef, gluLookAt, gluPerspective, glPushMatrix, glPopMatrix,
// Illuminacio: glMaterialfv, glMaterialfv, glMaterialf, glLightfv
// Texturazas: glGenTextures, glBindTexture, glTexParameteri, glTexImage2D, glTexEnvi, 
// Pipeline vezerles: glShadeModel, glEnable/Disable a kovetkezokre:
// GL_LIGHTING, GL_NORMALIZE, GL_DEPTH_TEST, GL_CULL_FACE, GL_TEXTURE_2D, GL_BLEND, GL_LIGHT[0..7]
//
// NYILATKOZAT
// ---------------------------------------------------------------------------------------------
// Nev    : VARGA BALAZS
// Neptun : IFAW8V
// ---------------------------------------------------------------------------------------------
// ezennel kijelentem, hogy a feladatot magam keszitettem, es ha barmilyen segitseget igenybe vettem vagy 
// mas szellemi termeket felhasznaltam, akkor a forrast es az atvett reszt kommentekben egyertelmuen jeloltem. 
// A forrasmegjeloles kotelme vonatkozik az eloadas foliakat es a targy oktatoi, illetve a 
// grafhazi doktor tanacsait kiveve barmilyen csatornan (szoban, irasban, Interneten, stb.) erkezo minden egyeb 
// informaciora (keplet, program, algoritmus, stb.). Kijelentem, hogy a forrasmegjelolessel atvett reszeket is ertem, 
// azok helyessegere matematikai bizonyitast tudok adni. Tisztaban vagyok azzal, hogy az atvett reszek nem szamitanak
// a sajat kontribucioba, igy a feladat elfogadasarol a tobbi resz mennyisege es minosege alapjan szuletik dontes.  
// Tudomasul veszem, hogy a forrasmegjeloles kotelmenek megsertese eseten a hazifeladatra adhato pontokat 
// negativ elojellel szamoljak el es ezzel parhuzamosan eljaras is indul velem szemben.
//=============================================================================================

#include <math.h>
#include <stdlib.h>

#if defined(WIN32) || defined(_WIN32) || defined(__WIN32__)
// MsWindows-on ez is kell
#include <windows.h>     
#endif // Win32 platform

#include <GL/gl.h>
#include <GL/glu.h>
// A GLUT-ot le kell tolteni: http://www.opengl.org/resources/libraries/glut/
#include <GL/glut.h>     

#define new new_nelkul_is_meg_lehet_csinalni

//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
// Innentol modosithatod...

//=============================================================

/*
 * Irodalomjegyzek
 * [Szir99] DR . SZIRMAY-KALOS LASZLO - SZAMITOGEPES GRAFIKA, 1999.
 */

#define W_HEAD_SIZE 0.03
#define W_HEAD_POINTS_NUM 4
#define W_TAIL_FULL_AMPL 0.03
#define W_TAIL_PERIODS_NUM 3
#define W_TAIL_FULL_LENGTH 0.15
#define W_TAIL_RESOLUTION 0.001
#define W_TAIL_POINTS_NUM 150 //FULL_LENGTH/TAIL_RESOLUTION
#define W_SPEED_DIV 36

//pálya (field) elemek
#define F_NUM_OF_ELEMENTS 7
#define F_LIFT_STEPPING 0.7

bool working = false;
float time = 0;

//(c) [Szir99]
typedef float Coord;

typedef struct {
    float R, G, B;

    void set(float r, float g, float b) {
        R = r;
        G = g;
        B = b;
    }
} Color;

class Point2D {
    Coord x, y;
public:

    Point2D(float x0 = 0, float y0 = 0) {
        x = x0;
        y = y0;
    }

    float& X() {
        return x;
    }

    float& Y() {
        return y;
    }

    void set(float _x, float _y) {
        x = _x;
        y = _y;
    }
};

class Lift {
    Point2D beginP, endP;
    int level; //0,1,2
public:

    Lift(Point2D a, Point2D b) {
        beginP = a;
        endP = b;
        level = 2;
    }

    bool moveUp() {
        if (level < 2) {

            beginP.Y() += F_LIFT_STEPPING;
            endP.Y() += F_LIFT_STEPPING;
            level++;

            return true;
        }
        return false;
    }

    bool moveDown() {
        if (level > 0) {

            beginP.Y() -= F_LIFT_STEPPING;
            endP.Y() -= F_LIFT_STEPPING;
            level--;

            return true;
        }
        return false;
    }

    Point2D getBeginP() const {
        return beginP;
    }

    Point2D getEndP() const {
        return endP;
    }

};

Point2D fieldElements[F_NUM_OF_ELEMENTS][2]; //2 pontból lesz egy szakasz
Lift lift1(Point2D(-0.6, 0.4), Point2D(-0.2, 0.4));
Lift lift2(Point2D(0.6, 0.4), Point2D(0.2, 0.4));

//(c) otlet: https://lists.sch.bme.hu/wws/arc/grafika/2011-09/msg00264.html

bool fequals(float f1, float f2) {
    if (fabs(f1 - f2) < 0.001) return true;
    return false;
}

class Worm {
    Point2D headPoints[W_HEAD_POINTS_NUM];
    Point2D tailPoints[W_TAIL_POINTS_NUM];
    float length;
    bool shorter;
    int toRight; //ha jobbra tart, akkor 1-gyel szorozzuk, ha balra, akkor -1-gyel
    Color color;

    void generateTailPoints() {

        float const multipl = 16 * pow(W_TAIL_PERIODS_NUM, 2);
        float const ampl = sqrt((pow(W_TAIL_FULL_LENGTH, 2) + multipl * pow(W_TAIL_FULL_AMPL, 2)
                - pow(length, 2)) / multipl);

        int c = 0;
        Point2D nosePos = headPoints[0];
        for (float i = 0.0; i < length; i += W_TAIL_RESOLUTION) {

            float x = i;
            float y = nosePos.Y() - ampl * sin(2 * W_TAIL_PERIODS_NUM * M_PI / length * x);

            if (toRight > 0) { //balra megy
                x = nosePos.X() - 2 * W_HEAD_SIZE - x;
            } else {
                x = nosePos.X() + x;
            }

            tailPoints[c++] = Point2D(x, y);
        }
    }

    void fall(Lift &lift) {
        float const deltaY = 0.01;
        int const noseIndex = 0;

        if (inLiftPipe(lift)) {

            //liftre esik v. a fsz-re?
            if (lift.getBeginP().Y() < headPoints[noseIndex].Y()) { //liftre
                setNosePos(Point2D(headPoints[noseIndex].X(),
                        lift.getBeginP().Y() + W_HEAD_SIZE + deltaY));
            } else { //fsz
                setNosePos(Point2D(headPoints[noseIndex].X(), -1.0 + W_HEAD_SIZE + deltaY));
            }
        }
    }

    void fallDetect() {

        fall(lift1);
        fall(lift2);

    }

    bool inLiftPipe(Lift const &lift) {
        int const lastPoint = length / W_TAIL_RESOLUTION;
        
        float const headX = headPoints[0].X();
        float const tailX = tailPoints[lastPoint].X();
        if ((headX > lift.getBeginP().X() &&
                headX < lift.getEndP().X() &&
                tailX > lift.getBeginP().X() &&
                tailX < lift.getEndP().X())
                ||
                (headX < lift.getBeginP().X() &&
                headX > lift.getEndP().X() &&
                tailX < lift.getBeginP().X() &&
                tailX > lift.getEndP().X()))
            return true;

        return false;
    }

    bool onTheLift(Lift &lift) {
        int const lastPoint = length / W_TAIL_RESOLUTION;

        if (((headPoints[0].X() > lift.getBeginP().X() &&
                headPoints[0].X() < lift.getEndP().X()) ||
                (tailPoints[lastPoint].X() > lift.getBeginP().X() &&
                tailPoints[lastPoint].X() < lift.getEndP().X()))
                ||
                ((headPoints[0].X() < lift.getBeginP().X() &&
                headPoints[0].X() > lift.getEndP().X()) ||
                (tailPoints[lastPoint].X() < lift.getBeginP().X() &&
                tailPoints[lastPoint].X() > lift.getEndP().X()))
                ) {
            return true;
        }

        return false;
    }

    bool onDiffLevels(Lift &lift) {
        float const deltaY = 0.01;
        if (fequals(headPoints[0].Y() - W_HEAD_SIZE, lift.getBeginP().Y() + deltaY)) {
            return false;
        }

        return true;
    }
public:

    Worm() {
        toRight = 1;
        shorter = true;
        length = W_TAIL_FULL_LENGTH;
    }

    void setColor(Color color) {
        this->color = color;
    }

    Color getColor() const {
        return color;
    }

    void setNosePos(Point2D _nosePos) {
        working = true;

        headPoints[0] = _nosePos;
        Point2D tmp(_nosePos.X() - W_HEAD_SIZE, _nosePos.Y() - W_HEAD_SIZE);
        headPoints[1] = tmp;
        tmp.set(_nosePos.X() - W_HEAD_SIZE * 2, _nosePos.Y());
        headPoints[2] = tmp;
        tmp.set(_nosePos.X() - W_HEAD_SIZE, _nosePos.Y() + W_HEAD_SIZE);
        headPoints[3] = tmp;

        generateTailPoints();

        working = false;
    }

    float getLength() const {
        return length;
    }

    Point2D getNosePos() const {
        return headPoints[0];
    }

    Point2D* getHeadPoints() {
        return headPoints;
    }

    Point2D* getTailPoints() {
        return tailPoints;
    }

    void setDir() {
        working = true;

        toRight *= -1;
        generateTailPoints();

        working = false;
    }

    void moveUp(Lift &lift) {
        if (onTheLift(lift) && onDiffLevels(lift))
            setNosePos(Point2D(headPoints[0].X(), headPoints[0].Y() + F_LIFT_STEPPING));
    }

    void moveDown(Lift &lift) {
        if (inLiftPipe(lift))
            setNosePos(Point2D(headPoints[0].X(), headPoints[0].Y() - F_LIFT_STEPPING));
    }

    void control(float ts, float te) {
        //step
        working = true;

        //mozgas animalasa
        if (shorter) {
            if (length - W_TAIL_FULL_LENGTH / W_SPEED_DIV
                    < W_TAIL_FULL_LENGTH / 2) {
                shorter = false;
            } else {
                length -= W_TAIL_FULL_LENGTH / W_SPEED_DIV;
                generateTailPoints();
            }
        } else {
            if (length + W_TAIL_FULL_LENGTH / W_SPEED_DIV
                    > W_TAIL_FULL_LENGTH) {
                shorter = true;
            } else {
                length += W_TAIL_FULL_LENGTH / W_SPEED_DIV;
                Point2D newPos(headPoints[0].X() + (W_TAIL_FULL_LENGTH * toRight) / W_SPEED_DIV,
                        headPoints[0].Y());
                setNosePos(newPos);
            }
        }

        //kilepesdetekt
        if (toRight > 0 && headPoints[0].X() > 1.0) {
            setDir();
        } else if (toRight < 0 && headPoints[2].X() < -1.0) {
            setDir();
        }

        //esesdetekt
        fallDetect();

        //utkozesdetekt

        working = false;
    }

};

Worm greenWorm;
Worm redWorm;

//pálya elemek rajzolása

void drawFieldElements() {
    glColor3f(1.0, 1.0, 1.0); // fehérrel
    glBegin(GL_LINES);

    for (int i = 0; i < F_NUM_OF_ELEMENTS; i++) {
        glVertex2f(fieldElements[i][0].X(), fieldElements[i][0].Y());
        glVertex2f(fieldElements[i][1].X(), fieldElements[i][1].Y());
    }

    glEnd();
}

void drawLift(Lift &lift) {
    glColor3f(1.0, 1.0, 1.0); // fehérrel
    glBegin(GL_LINES);

    for (int i = 0; i < 2; i++) {
        glVertex2f(lift.getBeginP().X(), lift.getBeginP().Y());
        glVertex2f(lift.getEndP().X(), lift.getEndP().Y());
    }

    glEnd();
}

void initWorm(Worm &w, Point2D nosePos, Color color) {

    //beállítani a színt
    w.setColor(color);

    //beállítani a fej pontját
    w.setNosePos(nosePos);
}

void drawWorm(Worm &w) {
    working = true;

    //megrajzolni a fejét
    glColor3f(w.getColor().R, w.getColor().G, w.getColor().B);
    glBegin(GL_POLYGON);
    for (int i = 0; i < W_HEAD_POINTS_NUM; i++) {
        glVertex2f(w.getHeadPoints()[i].X(), w.getHeadPoints()[i].Y());
    }
    glEnd();

    //megrajzolni a farkát
    int c = w.getLength() / W_TAIL_RESOLUTION;
    glBegin(GL_LINE_STRIP);
    for (int i = 0; i < c; i++) {
        glVertex2f(w.getTailPoints()[i].X(), w.getTailPoints()[i].Y());
    }
    glEnd();

    working = false;
}

void simulateWorld(float tstart, float tend) {
    float dt = 50;
    for (float ts = tstart; ts < tend; ts += dt) {
        float te;
        if (tend >= ts + dt) {
            te = ts + dt;
        } else {
            te = tend;
        }

        greenWorm.control(ts, te);
        redWorm.control(ts, te);
    }

}

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)

void onInitialization() {
    //szintek letrehozasa
    //felso szint
    fieldElements[0][0] = Point2D(-1.0, 0.40);
    fieldElements[0][1] = Point2D(-0.6, 0.40);

    fieldElements[1][0] = Point2D(-0.2, 0.40);
    fieldElements[1][1] = Point2D(0.2, 0.40);

    fieldElements[2][0] = Point2D(0.6, 0.40);
    fieldElements[2][1] = Point2D(1.0, 0.40);

    //kozepso szint
    fieldElements[3][0] = Point2D(-1.0, -0.30);
    fieldElements[3][1] = Point2D(-0.6, -0.30);

    fieldElements[4][0] = Point2D(-0.2, -0.30);
    fieldElements[4][1] = Point2D(0.2, -0.30);

    fieldElements[5][0] = Point2D(0.6, -0.30);
    fieldElements[5][1] = Point2D(1.0, -0.30);

    //also szint vegig
    fieldElements[6][0] = Point2D(-1.0, -1.00);
    fieldElements[6][1] = Point2D(1.0, -1.0);

    Point2D nose(-0.00, 0.44);
    Color c;

    //zöld giliszta
    c.set(0.0, 1.0, 0.0);
    initWorm(greenWorm, nose, c);

    //piros giliszta
    nose.X() = -0.00;
    nose.Y() = -0.26;
    c.set(1.0, 0.0, 0.0);
    initWorm(redWorm, nose, c);
    redWorm.setDir();
}

void onIdle() {
    working = true;
    float old_time = time;
    time = glutGet(GLUT_ELAPSED_TIME);

    simulateWorld(old_time, time);

    working = false;
    glutPostRedisplay();
}

void onDisplay() {
    glClearColor(0, 0, 0, 0); // torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

    if (!working) {
        drawFieldElements();
        drawLift(lift1);
        drawLift(lift2);
        drawWorm(greenWorm);
        drawWorm(redWorm);
    }

    glutSwapBuffers();
}

void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay(); // d beture rajzold ujra a kepet

    working = true;
    //lift iranyitasanak kezelese
    if (key == 'q') {
        if (lift1.moveUp()) {
            greenWorm.moveUp(lift1);
            redWorm.moveUp(lift1);
        }
    }
    if (key == 'a') {
        if (lift1.moveDown()) {
            greenWorm.moveDown(lift1);
            redWorm.moveDown(lift1);
        }
    }
    if (key == 'o') {
        if (lift2.moveUp()) {
            greenWorm.moveUp(lift2);
            redWorm.moveUp(lift2);
        }
    }
    if (key == 'l') {
        if (lift2.moveDown()) {
            greenWorm.moveDown(lift2);
            redWorm.moveDown(lift2);
        }
    }

    onIdle();
    working = false;

    glutPostRedisplay();
}

void onMouse(int button, int state, int x, int y) {
}

// ...Idaig modosithatod
//~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

// A C++ program belepesi pontja, a main fuggvenyt mar nem szabad bantani

int main(int argc, char **argv) {
    glutInit(&argc, argv); // GLUT inicializalasa
    glutInitWindowSize(600, 600); // Alkalmazas ablak kezdeti merete 600x600 pixel 
    glutInitWindowPosition(100, 100); // Az elozo alkalmazas ablakhoz kepest hol tunik fel
    glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE | GLUT_DEPTH); // 8 bites R,G,B,A + dupla buffer + melyseg buffer

    glutCreateWindow("Grafika hazi feladat"); // Alkalmazas ablak megszuletik es megjelenik a kepernyon

    glMatrixMode(GL_MODELVIEW); // A MODELVIEW transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();
    glMatrixMode(GL_PROJECTION); // A PROJECTION transzformaciot egysegmatrixra inicializaljuk
    glLoadIdentity();

    onInitialization(); // Az altalad irt inicializalast lefuttatjuk

    glutDisplayFunc(onDisplay); // Esemenykezelok regisztralasa
    glutMouseFunc(onMouse);
    glutIdleFunc(onIdle);
    glutKeyboardFunc(onKeyboard);

    glutMainLoop(); // Esemenykezelo hurok

    return 0;
}
