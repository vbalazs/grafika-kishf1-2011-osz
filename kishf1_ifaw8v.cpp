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

//(c) [Szir99]
typedef float Coord;

//(c) [Szir99]

typedef struct {
    float R, G, B;
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
};

//(c) otlet: https://lists.sch.bme.hu/wws/arc/grafika/2011-09/msg00264.html

bool fequals(float f1, float f2) {
    if (fabs(f1 - f2) < 0.001) return true;
    return false;
}

//egysegnegyzet
//-- kozepen 0.0, 0.0
//-- bal also sarok: -1.0, -1.0
//-- jobb felso sarok: 1.0, 1.0

bool working = false;

const int NUM_OF_FIELD_ELEMENTS = 9; //7 statikus szint-elem + 2 lift
const int LIFT_A_INDEX = 7;
const int LIFT_B_INDEX = 8;
const float liftSteppingPx = 0.1;

Point2D fieldElements[NUM_OF_FIELD_ELEMENTS][2]; //2 pontból lesz egy szakasz

bool isYOverflow(const float y, const bool positive) {
    if (fequals(y - 1.1, 0) && positive) return true; //lebeg?pontos szopófaktor!
    if (fequals(y + 1.1, 0) && !positive) return true;
    return false;
}

//pálya elemek rajzolása

void drawFieldElements() {
    glColor3f(1.0, 1.0, 1.0); // fehérrel
    glBegin(GL_LINES);

    for (int i = 0; i < NUM_OF_FIELD_ELEMENTS; i++) {
        glVertex2f(fieldElements[i][0].X(), fieldElements[i][0].Y());
        glVertex2f(fieldElements[i][1].X(), fieldElements[i][1].Y());
    }

    glEnd();
}

// Inicializacio, a program futasanak kezdeten, az OpenGL kontextus letrehozasa utan hivodik meg (ld. main() fv.)

void onInitialization() {
    //- egy giliszta letrehozasa
    //- giliszta mozgatása billentyure

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
    fieldElements[LIFT_A_INDEX][0] = Point2D(-0.6, 0.0);
    fieldElements[LIFT_A_INDEX][1] = Point2D(-0.2, 0.0);

    //masik lift ("B")
    fieldElements[LIFT_B_INDEX][0] = Point2D(0.2, 0.0);
    fieldElements[LIFT_B_INDEX][1] = Point2D(0.6, 0.0);

}

// Rajzolas, ha az alkalmazas ablak ervenytelenne valik, akkor ez a fuggveny hivodik meg

void onDisplay() {
    glClearColor(0, 0, 0, 0); // torlesi szin beallitasa
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // kepernyo torles

    if (!working) {
        //ujrarajzolasok
    }

    drawFieldElements();

    glutSwapBuffers();
}

void onKeyboard(unsigned char key, int x, int y) {
    if (key == 'd') glutPostRedisplay(); // d beture rajzold ujra a kepet

    //lift iranyitasanak kezelese
    if (key == 'q') {
        if (!isYOverflow(fieldElements[LIFT_A_INDEX][0].Y() + liftSteppingPx, true)) {
            fieldElements[LIFT_A_INDEX][0].Y() += liftSteppingPx;
            fieldElements[LIFT_A_INDEX][1].Y() += liftSteppingPx;
            glutPostRedisplay();
        }
    }
    if (key == 'a') {
        if (!isYOverflow(fieldElements[LIFT_A_INDEX][0].Y() - liftSteppingPx, false)) {
            fieldElements[LIFT_A_INDEX][0].Y() -= liftSteppingPx;
            fieldElements[LIFT_A_INDEX][1].Y() -= liftSteppingPx;
            glutPostRedisplay();
        }
    }
    if (key == 'o') {
        if (!isYOverflow(fieldElements[LIFT_B_INDEX][0].Y() + liftSteppingPx, true)) {
            fieldElements[LIFT_B_INDEX][0].Y() += liftSteppingPx;
            fieldElements[LIFT_B_INDEX][1].Y() += liftSteppingPx;
            glutPostRedisplay();
        }
    }
    if (key == 'l') {
        if (!isYOverflow(fieldElements[LIFT_B_INDEX][0].Y() - liftSteppingPx, false)) {
            fieldElements[LIFT_B_INDEX][0].Y() -= liftSteppingPx;
            fieldElements[LIFT_B_INDEX][1].Y() -= liftSteppingPx;
            glutPostRedisplay();
        }
    }
}

void onMouse(int button, int state, int x, int y) {
}

void onIdle() {
    long time = glutGet(GLUT_ELAPSED_TIME);
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
