/* Xuedan Fillmore
 * 22 June 2020
 * CSCI 4229 Computer Graphics
 * 
 * Project ~ Typewriter simulator
 * 
 * REFERENCES
 * https://www.glprogramming.com/red/chapter13.html
 * https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/gluPickMatrix.xml
 * https://community.khronos.org/t/i-want-to-draw-two-objects-then-just-move-one-of-them/16733/3
 * https://learnopengl.com/Lighting/Basic-Lighting
 */

#include "config.hpp"
#include "twobjects.hpp"

//============== global variables ===================//
// texture
unsigned int textures[36];
// lighting
float ambient   = 10.0; // percent ambient intensity 
float diffuse   = 50.0; // percent diffuse intensity 
float specular  = 0.0; // percent specular (reflective spot) intensity 
float shininess = 0.0; // pow2 shininess 
float lposy     = 5; // height of light source 
float lposxz    = 0; // radial position of light 
// viewing
int theta  = 0;   // azimuth (left/right) angle 
int phi    = -30;   // elevation (up/down) angle 
float dim  = 30.0;   // dimension / world size 
float aspr = 1;    // aspect ratio (viewport/window) 
int fov    = 60;   // field of view 
float camx = 0; // x eye pos (perspective) 
float camy = 20; // constant view height (perspective) 
float camz = 5; // z eye pos (perspective) 
// controls
bool figkey = 0;
bool capkey = 0;

// create the scene's source of "sunlight"
// copied from hw3
void lightsrc(double xposl, double yposl, double zposl)
{
    float emis[] = {0.0, 0.2, 0.8, 1.0};

    // save current transformations
    glPushMatrix();
    //  offset, scale and rotate
    glTranslated(xposl, yposl, zposl);
    glScaled(0.1, 0.1, 0.1);
    // color the source white
    glColor3f(1.0, 1.0, 1.0);
    glMaterialfv(GL_FRONT,GL_EMISSION, emis);
    
    // the polygon (tetrahedron) itself
    // coordinates assumed from https://en.wikipedia.org/wiki/Tetrahedron
    glBegin(GL_TRIANGLE_STRIP);
    glVertex3d( 0.9428,       0, -0.3333);
    glVertex3d(-0.4714,  0.8165, -0.3333);
    glVertex3d(-0.4714, -0.8165, -0.3333); // 3rd vertex close 1st side
    glVertex3d(      0,       0,       1); // 4th vertex close 2nd side
    glVertex3d(-0.4714,  0.8165, -0.3333); // repeat 2nd vertex to close 3rd side
    glVertex3d(-0.4714, -0.8165, -0.3333); // repeat 3rd vertex to close 4th side
    glEnd();

    // replace transofrmations
    glPopMatrix();
}

// display function called by GLUT
void display()
{
    // set up for new buffer
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT); // Clear current image
    glLoadIdentity(); // Reset prior transformations
    glEnable(GL_DEPTH_TEST); // Allow z-buffer

    // View mode specific settings and text panel
    // copied from hw3
    glRotatef(-phi,   1, 0, 0);
    glRotatef(-theta, 0, 1, 0);
    glTranslatef(-camx, -camy, -camz);
    glColor3f(0.8, 0.8, 0.8); // make text light grey
    glWindowPos2i(5, 5);
    gprint("1st Person Perspective Projection (FOV %d)", fov);

    // check for any errors during set up
    checkErrs("display::setup");

    // set light in scene
    // copied from hw3
    // translate intensity to rgba color vectors
    float amb = 0.01 * (float)ambient;
    float dif = 0.01 * (float)diffuse;
    float spe = 0.01 * (float)specular;
    float Ambient[]  = {amb, amb, amb, 1.0};
    float Diffuse[]  = {dif, dif, dif, 1.0};
    float Specular[] = {spe, spe, spe, 1.0};
    //  Light position
    float lightposition[] = {dim * cosd(lposxz) /6, lposy, dim * sind(lposxz) /6, 1.0};
    // draw source object for the light
    lightsrc(lightposition[0], lightposition[1], lightposition[2]);
    //  Tell OpenGL to normalize normal vectors
    glEnable(GL_NORMALIZE);
    // Enable lighting
    glEnable(GL_LIGHTING);
    // set ambient and diffuse color materials
    glColorMaterial(GL_FRONT_AND_BACK,GL_AMBIENT_AND_DIFFUSE);
    glEnable(GL_COLOR_MATERIAL);
    // Enable light 0
    glEnable(GL_LIGHT0);
    // Set ambient, diffuse, specular components and position of light 0
    glLightfv(GL_LIGHT0, GL_AMBIENT,  Ambient);
    glLightfv(GL_LIGHT0, GL_DIFFUSE,  Diffuse);
    glLightfv(GL_LIGHT0, GL_SPECULAR, Specular);
    glLightfv(GL_LIGHT0, GL_POSITION, lightposition);

    checkErrs("display::lighting");

    // enable texture application
    // glEnable(GL_TEXTURE_2D);
    // glTexEnvi(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);

    // build typewriter, passing texture array as argument
    Typewriter tw;
    tw.buildTypewriter(textures);

    checkErrs("display::twInit");

    // disable individual aspects of scene
    glDisable(GL_TEXTURE_2D);
    glDisable(GL_LIGHTING);

    // clean up and redisplay
    checkErrs("display");
    glFlush();
    project(fov, aspr, dim);
    glutSwapBuffers();
}

void keybindings(unsigned char key, int xpos, int ypos)
{
    if (key == 27 || key == 113)
    { // q or ESC - exit
        exit(0);
    }
    else if (key == 114)
    { // r - reset view
        camx = 0;
        camy = 20;
        camz = 5;
        dim = 30.0;
        aspr = 1.0;
        fov = 60;
    }
    else if (key == 119 || key == 97 || key == 115 || key == 100)
    { // w, a, s, d lateral movement of camera/eye
        // copied from hw3 ~ Pentamollis Project
        float movspeed = 2.0;
        switch (key)
        {
            case 119: // w - lateral forward
                camx += cosd(theta+90) / movspeed;
                camz -= sind(theta+90) / movspeed;
                break;
            case 115: // s - lateral backward
                camx += cosd(theta+270) / movspeed;
                camz -= sind(theta+270) / movspeed;
                break;
            case 97:  // a - lateral left
                camx += cosd(theta+180) / movspeed;
                camz -= sind(theta+180) / movspeed;
                break;
            case 100: // d - lateral right
                camx += cosd(theta) / movspeed;
                camz -= sind(theta) / movspeed;
                break;
        }
    }
    else if (key == 120 || key == 122)
    { // z and x to rise/fall respectively
        if (key == 120) camy -= 2.5; // x
        else camy += 2.5; // z
    }
    else if (key == 102 || key == 70)
    { // f/F increase/decrease field of view
        if (key == 102)
        {
            fov -= 2;
        }
        else
        {
            fov += 2;
        }

        /* maintain reasonable fov range */
        if (fov < 45) fov = 46;
        if (fov > 81) fov = 80;
    }

    // tell GLUT to redisplay after key press
    project(fov, aspr, dim);
    glutPostRedisplay();
}

void specialkeybindings(int key, int xpos, int ypos)
{
    switch(key)
    { // azimuth ~ longitude / elevation ~ latitude
        case GLUT_KEY_RIGHT:
            theta -= 2.5;      // increase longitudinal view 5 deg
            break;
        case GLUT_KEY_LEFT:
            theta += 2.5;      // decrease longitudinal view 5 deg
            break;
        case GLUT_KEY_UP:
            phi += 2.5;       // increase latitudinal view 5 deg
            if (phi > 70) phi = 70;
            break;
        case GLUT_KEY_DOWN:
            phi -= 2.5;       // decrease latitudinal view 5 deg
            if (phi < -85) phi = -85;
            break;
    }

    // Maintain angle between 0 and 360 deg
    theta %= 360;
    phi  %= 360;


    // tell GLUT to redisplay after key press
    project(fov, aspr, dim);
    glutPostRedisplay();
}

void reshape(int width, int height)
{
    // ratio width:height
    aspr = (height > 0) ? (double)width/height : 1;

    // set viewport to entire window
    glViewport(0, 0, width, height);

    // update projection
    project(fov, aspr, dim);
}

void idle()
{
    // elapsed time wrt 2 secs
    double t = glutGet(GLUT_ELAPSED_TIME)/2000.0;
    lposxz = fmod(90*t, 360.0);

    // redisplay scene (light is moving)
    glutPostRedisplay();
}

int main(int argc, char* argv[])
{
    srand((unsigned)time(NULL));

    // Initialize GLUT, process user params
    glutInit(&argc, argv);

    // Display mode requests
    glutInitDisplayMode(GLUT_RGB | GLUT_DEPTH | GLUT_DOUBLE);

    // Start with 700x500 px window
    glutInitWindowSize(700, 500);

    // Create window with name
    glutCreateWindow("Xuedan Fillmore | Typewriter");

    // register 'display' as scene drawing fn
    glutDisplayFunc(display);

    // register 'reshape' as window resizing fn
    glutReshapeFunc(reshape);

    // idle function
    glutIdleFunc(idle);

    // register key bindings for keyboard and arrows
    glutSpecialFunc(specialkeybindings);
    glutKeyboardFunc(keybindings);

    // check for initialization errors
    checkErrs("main::init");

    // load texture files to be used
    textures[0]  = loadTexture("./tex/keyrim_metal.bmp");
    textures[1]  = loadTexture("./tex/body_plastic2.bmp");
    // textures[2]  = loadTexture("./tex/.bmp"); // NO TEXTURE HERE
    textures[3]  = loadTexture("./tex/keys_0cap.bmp");
    textures[4]  = loadTexture("./tex/keys_0fig.bmp");
    textures[5]  = loadTexture("./tex/keys_q.bmp");
    textures[6]  = loadTexture("./tex/keys_w.bmp");
    textures[7]  = loadTexture("./tex/keys_e.bmp");
    textures[8]  = loadTexture("./tex/keys_r.bmp");
    textures[9]  = loadTexture("./tex/keys_t.bmp");
    textures[10] = loadTexture("./tex/keys_y.bmp");
    textures[11] = loadTexture("./tex/keys_u.bmp");
    textures[12] = loadTexture("./tex/keys_i.bmp");
    textures[13] = loadTexture("./tex/keys_o.bmp");
    textures[14] = loadTexture("./tex/keys_p.bmp");
    textures[15] = loadTexture("./tex/keys_a.bmp");
    textures[16] = loadTexture("./tex/keys_s.bmp");
    textures[17] = loadTexture("./tex/keys_d.bmp");
    textures[18] = loadTexture("./tex/keys_f.bmp");
    textures[19] = loadTexture("./tex/keys_g.bmp");
    textures[21] = loadTexture("./tex/keys_h.bmp");
    textures[22] = loadTexture("./tex/keys_j.bmp");
    textures[23] = loadTexture("./tex/keys_k.bmp");
    textures[24] = loadTexture("./tex/keys_l.bmp");
    textures[25] = loadTexture("./tex/keys_0question.bmp");
    textures[26] = loadTexture("./tex/keys_z.bmp");
    textures[27] = loadTexture("./tex/keys_x.bmp");
    textures[28] = loadTexture("./tex/keys_c.bmp");
    textures[29] = loadTexture("./tex/keys_v.bmp");
    textures[30] = loadTexture("./tex/keys_b.bmp");
    textures[31] = loadTexture("./tex/keys_n.bmp");
    textures[32] = loadTexture("./tex/keys_m.bmp");
    textures[33] = loadTexture("./tex/keys_0comma.bmp");
    textures[34] = loadTexture("./tex/keys_0period.bmp");
    textures[35] = loadTexture("./tex/keys_0minus.bmp");

    // check for texure loading errors
    checkErrs("main::textures");

    // Pass control to GLUT for user interaction
    glutMainLoop();

    return 0;
}