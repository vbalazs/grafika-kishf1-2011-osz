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
#define W_TAIL_FULL_LENGTH 0.15
#define W_TAIL_RESOLUTION 0.001
#define W_TAIL_POINTS_NUM 150 //FULL_LENGTH/TAIL_RESOLUTION

//(c) [Szir99]
typedef float Coord;
//(c) [Szir99]

typedef struct {
    float R, G, B;

    void set(float r, float g, float b) {
        R = r;
        G = g;
        B = b;
    }
} Color;

//(c) [Szir99]

class Point2D {
    Coord x, y;
public:

    Point2D(float x0 = 0, float y0 = 0) {
        x = x0;
        y = y0;
    }

    Point2D operator-() {
        return Point2D(-x, -y);
    }

    Point2D operator+(Point2D p) {
        return Point2D(x + p.x, y + p.y);
    }

    Point2D operator*(float s) {
        return Point2D(x*s, y * s);
    }

    float Length() {
        return sqrt(x * x + y * y);
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

class Worm {
    Point2D headPoints[W_HEAD_POINTS_NUM];
    Point2D tailPoints[W_TAIL_POINTS_NUM];
    bool shortMode; //rövid állapot?
    bool toLeft; //balra tart?
    Color color;
public:

    Worm() {
        toLeft = false;
        shortMode = false;
    }

    bool isShortMode() const {
        return shortMode;
    }

    void setShortMode(bool _shortMode) {
        shortMode = _shortMode;
    }

    void setColor(Color color) {
        this->color = color;
    }

    Color getColor() const {
        return color;
    }

    void setNosePos(Point2D _nosePos) {
        headPoints[0] = _nosePos;

        Point2D tmp(_nosePos.X() - W_HEAD_SIZE, _nosePos.Y() - W_HEAD_SIZE);
        headPoints[1] = tmp;
        tmp.set(_nosePos.X() - W_HEAD_SIZE * 2, _nosePos.Y());
        headPoints[2] = tmp;
        tmp.set(_nosePos.X() - W_HEAD_SIZE, _nosePos.Y() + W_HEAD_SIZE);
        headPoints[3] = tmp;
    }

    Point2D getNosePos() const {
        return headPoints[0];
    }

    Point2D* getHeadPoints() {
        return headPoints;
    }

    bool isToLeft() const {
        return toLeft;
    }

    void setToLeft(bool _toLeft) {
        toLeft = _toLeft;
    }

};

//(c) otlet: https://lists.sch.bme.hu/wws/arc/grafika/2011-09/msg00264.html

bool fequals(float f1, float f2) {
    if (fabs(f1 - f2) < 0.001) return true;
    return false;
}

bool isYOverflow(const float y, const bool positive) {
    if (fequals(y - 1.1, 0) && positive) return true; //lebegopontos szopófaktor!
    if (fequals(y + 1.1, 0) && !positive) return true;
    return false;
}

bool working = false;

//pálya (field) elemek
#define F_NUM_OF_ELEMENTS 9 //7 statikus szint-elem + 2 lift
#define F_LIFT_A_INDEX 7
#define F_LIFT_B_INDEX 8
#define F_LIFT_STEPPING 0.1

Point2D fieldElements[F_NUM_OF_ELEMENTS][2]; //2 pontból lesz egy szakasz

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

void initWorm(Worm &w, Point2D nosePos, Color color) {

    //beállítani a színt
    w.setColor(color);

    //beállítani a fej pontját
    w.setNosePos(nosePos);

    //
}

void drawWorm(Worm &w) {
    working = true;

    //TODO!! a számolásokat nem itt kéne, csak a kirajzolást!

    //megrajzolni a fejét
    glColor3f(w.getColor().R, w.getColor().G, w.getColor().B);
    glBegin(GL_POLYGON);
    glVertex2f(w.getNosePos().X(), w.getNosePos().Y());
    glVertex2f(w.getNosePos().X() - W_HEAD_SIZE, w.getNosePos().Y() - W_HEAD_SIZE);
    glVertex2f(w.getNosePos().X() - W_HEAD_SIZE * 2, w.getNosePos().Y());
    glVertex2f(w.getNosePos().X() - W_HEAD_SIZE, w.getNosePos().Y() + W_HEAD_SIZE);
    glEnd();

    //megrajzolni a farkát
    glBegin(GL_LINE_STRIP);
    float ampl = 0.03;
    float k = 3;
    float xlen = W_TAIL_FULL_LENGTH;
    if (w.isShortMode()) {
        ampl = 0.030121823;
        xlen = W_TAIL_FULL_LENGTH / 2;
    }
    for (float i = 0.001; i < xlen; i += 0.001) {
        float x = i;
        float y = w.getNosePos().Y() - ampl * sin(2 * k * M_PI / xlen * x);

        if (w.isToLeft()) { //balra megy
            x = w.getNosePos().X() + x;
        } else {
            x = w.getNosePos().X() - 2 * W_HEAD_SIZE - x;
        }

        glVertex2f(x, y);
    }

    glEnd();

    working = false;
}

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)

void onInitialization() {
    //szintek letrehozasa
    //felso szint
    fieldElements[0][0] = Point2D(-1.0, 0.30);
    fieldElements[0][1] = Point2D(-0.6, 0.30);

    fieldElements[1][0] = Point2D(-0.2, 0.30);
    fieldElements[1][1] = Point2D(0.2, 0.30);

    fieldElements[2][0] = Point2D(0.6, 0.30);
    fieldElements[2][1] = Point2D(1.0, 0.30);

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

    //egyik lift ("A")
    fieldElements[F_LIFT_A_INDEX][0] = Point2D(-0.6, 0.0);
    fieldElements[F_LIFT_A_INDEX][1] = Point2D(-0.2, 0.0);

    //masik lift ("B")
    fieldElements[F_LIFT_B_INDEX][0] = Point2D(0.2, 0.0);
    fieldElements[F_LIFT_B_INDEX][1] = Point2D(0.6, 0.0);

    Point2D nose(-0.00, 0.34);
    Color c;

    //zöld giliszta
    c.set(0.0, 1.0, 0.0);
    initWorm(greenWorm, nose, c);

    //piros giliszta
    nose.X() = -0.00;
    nose.Y() = -0.26;
    c.set(1.0, 0.0, 0.0);
    initWorm(redWorm, nose, c);
    redWorm.setToLeft(true);
}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg

void onDisplay() {
    glClearColor(0, 0, 0, 0); // torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

    if (!working) {
        //ujrarajzolasok
        drawFieldElements();
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
        if (!isYOverflow(fieldElements[F_LIFT_A_INDEX][0].Y() + F_LIFT_STEPPING, true)) {
            fieldElements[F_LIFT_A_INDEX][0].Y() += F_LIFT_STEPPING;
            fieldElements[F_LIFT_A_INDEX][1].Y() += F_LIFT_STEPPING;
        }
    }
    if (key == 'a') {
        if (!isYOverflow(fieldElements[F_LIFT_A_INDEX][0].Y() - F_LIFT_STEPPING, false)) {
            fieldElements[F_LIFT_A_INDEX][0].Y() -= F_LIFT_STEPPING;
            fieldElements[F_LIFT_A_INDEX][1].Y() -= F_LIFT_STEPPING;
        }
    }
    if (key == 'o') {
        if (!isYOverflow(fieldElements[F_LIFT_B_INDEX][0].Y() + F_LIFT_STEPPING, true)) {
            fieldElements[F_LIFT_B_INDEX][0].Y() += F_LIFT_STEPPING;
            fieldElements[F_LIFT_B_INDEX][1].Y() += F_LIFT_STEPPING;
        }
    }
    if (key == 'l') {
        if (!isYOverflow(fieldElements[F_LIFT_B_INDEX][0].Y() - F_LIFT_STEPPING, false)) {
            fieldElements[F_LIFT_B_INDEX][0].Y() -= F_LIFT_STEPPING;
            fieldElements[F_LIFT_B_INDEX][1].Y() -= F_LIFT_STEPPING;
        }
    }
    if (key == 'y') {
        greenWorm.setShortMode(!greenWorm.isShortMode());
    }
    if (key == 'x') {
        Point2D newPos(greenWorm.getNosePos().X() + W_TAIL_FULL_LENGTH / 2, greenWorm.getNosePos().Y());
        greenWorm.setShortMode(!greenWorm.isShortMode());
        greenWorm.setNosePos(newPos);
    }
    working = false;

    glutPostRedisplay();
}

void onMouse(int button, int state, int x, int y) {
}

void onIdle() {
    working = true;
    long time = glutGet(GLUT_ELAPSED_TIME);
    //    	Az állapot frissítése az eltelt ido függvényében
    //		Eltelt ido idoszeletekre bontása
    //			for minden idoszelet
    //				Objektumok pozíciójának számolása
    //				Ütközésvizsgálat - Ütközés hatásának beállítása
    //	glutPostRedisplay
    working = false;
    glutPostRedisplay();
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
