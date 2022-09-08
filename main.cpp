/*I created a freeze game icon. This Icon exists on the screen of 7 seconds and it moves randomly throughout
 * the screen. If the player shoots or runs into it the asteroids are frozen for 10 seconds and the user can destroye them.
 * I also made a Nuclear Icon that exists for 5 seconds and roams the screen randomly. When it is shot or ran into a barrage
 * of bullets originating from the center of the screen destroy asteroids/icons on the screen. Then when the UFO is destroyed
 * 4 cows are thrown out and spin off of the screen. This meant I had to create classes for the icons and derived classes for
 * the specific icons, figure out a timer for each icon, how to freeze the asteroids but still make them destroyable, create
 * classes for the cows, and other code parts below.*/

#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <time.h>
#include <list>
#include <cmath>
#include <stdio.h>
#include <iostream>
#include <thread>

using namespace sf;

const int W = 1200;
const int H = 800;

float DEGTORAD = 0.017453f;

/*contains the current frame, speed, sprite, and specs of the frams stored in a vector*/
class Animation
{
public:
    float Frame, speed;
    Sprite sprite;
    std::vector<IntRect> frames;

    Animation(){} //default constructor

    Animation (Texture &t, int x, int y, int w, int h, int count, float Speed) //other constructor sets frame to 0, input speed, and list of frame specs
    {
        Frame = 0;
        speed = Speed;

        for (int i=0;i<count;i++)
            frames.push_back( IntRect(x+i*w, y, w, h)  );

        sprite.setTexture(t); //sets the sprite's texture to the input texture
        sprite.setOrigin(w/2,h/2); //set true origin
        sprite.setTextureRect(frames[0]); //set initial texture to the first frame
    }

    /*changes the frame based on the speed and the acutal texture*/
    void update()
    {
        Frame += speed;
        int n = frames.size();
        if (Frame >= n) Frame -= n; //if greater than number frames, loop back to initial frame
        if (n>0) sprite.setTextureRect( frames[int(Frame)] ); //truncate the frame to access frames based ons peed
    }

    bool isEnd()
    {
        return Frame+speed>=frames.size(); //returns whether or not the next frame is greater than the number of frames
    }

};

/*Entity (base class) contains global x, global y, changes in x and y, and global R and angle vars. id name, and an associated animation object */
class Entity
{
public:
    float x,y,dx,dy,R,angle;
    bool life;
    std::string name;
    Animation anim;

/*constructor sets life to true*/
    Entity()
    {
        life=1;
    }

/*takes associated animation, x, y, angle, and radius as an arguement, and simply sets the member vars to these values*/
    void settings(Animation &a,int X,int Y,float Angle=0,int radius=1)
    {
        anim = a;
        x=X; y=Y;
        angle = Angle;
        R = radius;
    }

//virtual base update func, to be overriden by derived classes
    virtual void update(){};

//compiles all of rendering/drawing tasks of the entity into one function for ease
    void draw(RenderWindow &app)
    {
        anim.sprite.setPosition(x,y);
        anim.sprite.setRotation(angle+90);
        if(name == "UFO"){
            anim.sprite.setRotation(angle);
        }
        app.draw(anim.sprite);

        CircleShape circle(R);
        circle.setFillColor(Color(255,0,0,170));
        circle.setPosition(x,y);
        circle.setOrigin(R,R);
        //app.draw(circle);
    }

//virtual base class entity destructor, meant to be overriden by derived classes
    virtual ~Entity(){};
};

//inherits all member vars from entity, defines specific constructor and concrete update function and concrete defualt destructor
class asteroid: public Entity
{
public:
    static int countAsteroids;
    static bool asteroidsfrozen;
    asteroid() //create random change in x and y
    {
        dx=rand()%8-4;
        dy=rand()%8-4;
        name="asteroid";
        countAsteroids++;
    }

    void  update()
    {
        if(!asteroidsfrozen){
            x+=dx; //update x and y based on random change in x and y
            y+=dy;

            //leads to reflected opposite boundary effect
            if (x>W) x=0;  if (x<0) x=W;
            if (y>H) y=0;  if (y<0) y=H;
        }
    }
    ~asteroid(){
        countAsteroids--;
    }
};

/* inherits from Entity class, as a derived class. contains a static UFOcount variable and and an associated getter func
 * The ufo object travels horizontally with no vertical movement, and dies after exiting the boudnaries, */
class UFO: public Entity
{
public:
    static int UFOcount;
    UFO(){ //create random change in x and y
        dx = rand() % 3 + 2; //randomly generated change in x
        dy = 0;
        name = "UFO";
        UFOcount++;
    }
    ~UFO(){
        UFOcount--;
    }
    static int get_number_UFOs() {return UFOcount;};
    void update()
    {
        x+=dx; //update x and y based on random change in x and y
        y+=dy;

        //ends life if it exits boundary
        if (x>W || x<0 || y>H || y<0){
            life=0;
        }
    }

};
/* Cow class derived from entity class and has a random horizontal and vertical speed, with a varying change in angle, and
 * dies after leaving the screen's borders. When the UFO class object is destroyed cows are released that implode with
 * the asteroids acting as an aid to the player.*/
class Cow: public Entity{
public:
    Cow(){
        name = "Cow";
        dx = rand() % 4 - 2;
        dy = rand() % 4 - 2;
        angle = rand() % 360;
    }
    void update(){
        x+=dx;
        y+=dy;
        angle += (rand() % 6 + 4) % 360;
        if (x>W || x<0 || y>H || y<0){
            life=0;
        }
    }
};

/*Icon class is dervied from Entity class, and theoretically virtual with a virtual update function leading to polymorphism
 * from the other derived Icon classes*/
class Icon: public Entity
{
public:
    virtual void update(){};
};

/*WinterIcon class is derived from Icon class, and contains countWinter (number of WinterIcon objects) and a WinterWait (
 * whether the freeze effect is occuring), and a polymorphism using the update function. The WinterIcon freezes the asteroid
 * for 10 seconds if destroyed by collision or bullet.*/
class WinterIcon: public Icon{
public:
    static int countWinter;
    static bool WinterWait;

    WinterIcon(){
        name = "WinterIcon";
        countWinter++;
        dx = rand() % 6 -3;
        dy = rand() % 6 -3;
    }
    ~WinterIcon(){
        countWinter--;
    };
    void update(){
        if(rand()%150 == 1){ //random changes in speed
            dx = rand() % 6 - 3;
            dy = rand() % 6 - 3;
        }
        x+=dx;
        y+=dy;
        if(rand()%15 == 1){ //random changes in direction produce a random path
            angle = rand() % 360; //random angle changes not working
        }
        //reflected screen boundaries for the WinterIcon
        if (x>W) x=0; if (x<0) x=W;
        if (y>H) y=0; if (y<0) y=H;
    }
};
/*the NuclearIcon class derived from the Icon class, has a NuclearCount (number of Nuclear Icons) static member var, and uses
 * polymorphism to define the behavior of the update func. The NuclearIcon when destroyed using a collision or bullet releases
 * a barrage of bullets from the center.*/
class NuclearIcon: public Icon{
public:
    static int NuclearCount;
    NuclearIcon(){
        name = "NuclearIcon";
        dx = rand() % 6 - 3; //randomly defined speed at instantiation
        dy = rand() % 6 - 3;
        NuclearCount++;
    }
    ~NuclearIcon(){
        NuclearCount--;
    }
    void update(){
        if(rand()%150 == 1){ //randomly changing speed
            dx = rand() % 6 - 3;
            dy = rand() % 6 - 3;
        }
        x+=dx;
        y+=dy;
        if(rand()%50 == 1){ //randomly changing angle, make a random path
            angle = rand() % 360;
        }
        //reflective boundary
        if (x>W) x=0; if (x<0) x=W;
        if (y>H) y=0; if (y<0) y=H;
    }
};

//inherits member vars from entity class, provides concrete update function, and conrete default deconstructor
class bullet: public Entity
{
public:
    bullet()
    {
        name="bullet"; //bullet id
    }

    void  update()
    {
        //find the changed in x and y associated with the angular path defined by angle
        dx=cos(angle*DEGTORAD)*6;
        dy=sin(angle*DEGTORAD)*6;
        // angle+=rand()%6-3;
        //increment x and y
        x+=dx;
        y+=dy;

        //if bullet leaves the screen's boudnaries set life to false
        if (x>W || x<0 || y>H || y<0) life=0;
    }

};


// inherits member vars from Entity, but adds bool thrust. Concrete udpate and default deconstructor
class player: public Entity
{
public:
    bool thrust;

    player()
    {
        name="player";
    }

    void update()
    {
        if (thrust) //if thrust, find change in x and y associated with the angular path associated with angle
        { dx+=cos(angle*DEGTORAD)*0.2;
            dy+=sin(angle*DEGTORAD)*0.2; }
        else //de-acceleration
        { dx*=0.99;
            dy*=0.99; }

        int maxSpeed=15;
        float speed = sqrt(dx*dx+dy*dy); //calc speed
        if (speed>maxSpeed) // if speed passes max speed, decrease speed in ratio with the speed to maxspeed
        { dx *= maxSpeed/speed;
            dy *= maxSpeed/speed; }

        x+=dx;
        y+=dy;

        //opposite reflected boundary effect
        if (x>W) x=0; if (x<0) x=W;
        if (y>H) y=0; if (y<0) y=H;
    }

};

/* takes two pointers to an entity or derived class, and checks if they're global positions intersect and returns a bool if true or false*/
bool isCollide(Entity *a,Entity *b)
{
    return (b->x - a->x)*(b->x - a->x)+
           (b->y - a->y)*(b->y - a->y)<
           (a->R + b->R)*(a->R + b->R);
}


int asteroid::countAsteroids = 0;
int WinterIcon::countWinter = 0;
bool asteroid::asteroidsfrozen = false;
bool WinterIcon::WinterWait = false;
int NuclearIcon::NuclearCount = 0;
int UFO::UFOcount = 0;

int main()
{
    //seed random number generator
    srand(time(0));
    auto winter_start = std::chrono::system_clock::now(); //seed the winter_start, time WinterIcon created, and time NuclearIcon created variables
    auto WinterIconCreate = std::chrono::system_clock::now();
    auto NuclearIconCreate = std::chrono::system_clock::now();

//    bool ufoexists = false;
    int numufos;
    bool musicplaying = false;

    RenderWindow app(VideoMode(W, H), "Asteroids!");
    app.setFramerateLimit(60);

    Texture t1,t2,t3,t4,t5,t6,t7,t8,t9,t10,t12; //initialize the textures used in the program, no error checking??
    t1.loadFromFile("images/spaceship.png");
    t2.loadFromFile("images/background.jpg");
    t3.loadFromFile("images/explosions/type_C.png");
    t4.loadFromFile("images/rock.png");
    t5.loadFromFile("images/fire_blue.png");
    t6.loadFromFile("images/rock_small.png");
    t7.loadFromFile("images/explosions/type_B.png");
    t8.loadFromFile("images/ufo.png"); //ufo illustration made by Freepik, license: free to use with credit
    t9.loadFromFile("images/air-conditioner.png"); //"air-conditioner" illustration made by Freepik, license: free to use with credit
    t10.loadFromFile("images/nuclear-power.png"); //"nuclear-power" illustration made by Icongeek26, license: free to use with credit
    t12.loadFromFile("images/cow.png"); //cow illustration made by Victoruler, license: free to use with credit


    t1.setSmooth(true);
    t2.setSmooth(true);
    t7.setSmooth(true);
    t8.setSmooth(true);
    t9.setSmooth(true);
    t10.setSmooth(true);
    t12.setSmooth(true);

    Sprite background(t2);

    //instantiate the animation objects associated with the moving objects in the game
    Animation sExplosion(t3, 0,0,256,256, 48, 0.5);
    Animation sRock(t4, 0,0,64,64, 16, 0.2);
    Animation sRock_small(t6, 0,0,64,64, 16, 0.2);
    Animation sBullet(t5, 0,0,32,64, 16, 0.8);
    Animation sPlayer(t1, 40,0,40,40, 1, 0);
    Animation sPlayer_go(t1, 40,40,40,40, 1, 0);
    Animation sExplosion_ship(t7, 0,0,192,192, 64, 0.5);
    Animation sUFO(t8, 0, 0, 64, 64, 1, 0);
    Animation sFreezeIcon(t9, 0, 0, 60, 60, 1, 0);
    Animation sNuclearIcon(t10, 0, 0, 50, 50, 1, 0);
    Animation sCow(t12, 0, 0, 75, 75, 1, 0);


    Music music;
//    "547113 synth up" by rygerxini, license: free to use as long as a credit is provided.
    if(!music.openFromFile("sounds/547113__rvgerxini__synth-up.ogg")){
        return EXIT_FAILURE;
    }
    music.setLoop(true);

    //create list of all entities and derived class used in the program
    std::list<Entity*> entities;

    //instantiate 15 asteroids at the beginning of the game
    for(int i=0;i<15;i++)
    {
        asteroid *a = new asteroid();
        a->settings(sRock, rand()%W, rand()%H, rand()%360, 25);
        entities.push_back(a); //add to entities list
    }

    player *p = new player(); //create the player object for the player rocket, and set its settings
    p->settings(sPlayer, W/2, H/2, 0, 20); //start at the middle of the screen
    entities.push_back(p);

    /////main loop/////
    while (app.isOpen())
    {
        Event event;
        while (app.pollEvent(event))
        {
            if (event.type == Event::Closed)
                app.close();

            if (event.type == Event::KeyPressed)
                if (event.key.code == Keyboard::Space) //if space generate bullet objects
                {
                    bullet *b = new bullet();
                    b->settings(sBullet,p->x,p->y,p->angle,10); //xpos, ypos, and angle are shared with the player rocket at initialization
                    entities.push_back(b);
                }
        }

        if (Keyboard::isKeyPressed(Keyboard::Right)) p->angle+=3; //if right, rotate player's ship by 3 to the right
        if (Keyboard::isKeyPressed(Keyboard::Left))  p->angle-=3; //if left, rotate player's ship by 3 to the left
        if (Keyboard::isKeyPressed(Keyboard::Up)) p->thrust=true; //if player wants thrust set thrust to true
        else p->thrust=false;


////COLLISIONS////
        for(auto a:entities)
            for(auto b:entities)
            {
                if (a->name=="asteroid" && b->name=="bullet")
                    if ( isCollide(a,b) ) //if an asteroid and bullet collide set life of both to zero
                    {
                        a->life=false;
                        b->life=false;

                        //create entity with explosion animation, at the collision site.
                        Entity *e = new Entity();
                        e->settings(sExplosion,a->x,a->y);
                        e->name="explosion";
                        entities.push_back(e);


                        //if the asteroid involved is a "large" asteroid it should break up into 2 smaller asteroids
                        for(int i=0;i<2;i++)
                        {
                            if (a->R==15) continue;
                            Entity *e = new asteroid();
                            e->settings(sRock_small,a->x,a->y,rand()%360,15);
                            entities.push_back(e);
                        }

                    }
                //if player rocket and asteroid collide, set life of asteroid to false only, and reset player values to default
                if (a->name=="player" && b->name=="asteroid")
                    if ( isCollide(a,b) )
                    {
                        b->life=false;

                        Entity *e = new Entity(); //create explosion
                        e->settings(sExplosion_ship,a->x,a->y);
                        e->name="explosion";
                        entities.push_back(e);

                        p->settings(sPlayer,W/2,H/2,0,20);
                        p->dx=0; p->dy=0;
                    }
                if(a->name == "bullet" && b->name == "UFO"){
                    if(isCollide(a,b)){ //if a bullet object collides with a ufo object, both bullet and ufo have life set to false
                        a->life = false;
                        b->life = false;

                        Entity *e = new Entity(); //explosion object instantiated at the position of impact
                        e->settings(sExplosion, a->x, b->y);
                        e->name="explosion";
                        entities.push_back(e);

                        for(int i = 0; i < 4; i++){ //create 4 cow objects at the position of impact, with a random angle
                            Cow *c = new Cow();
                            c->settings(sCow, a->x, b->y, rand() % 360, 75/2);
                            entities.push_back(c);
                        }
                    }
                }
                if(a->name == "player" && b->name == "UFO"){
                    if(isCollide(a,b)){ //if the player collides with the UFO, set the life of the UFO to false
                        b->life = false;

                        Entity *e = new Entity(); //create an explosion at the position of impact
                        e->settings(sExplosion, a->x, b->y);
                        e->name="explosion";
                        entities.push_back(e);

                        for(int i = 0; i < 4; i++){ //4 cow objects are created at the site of impact with a random angle
                            Cow *c = new Cow();
                            c->settings(sCow, a->x, b->y, rand() % 360, 75/2);
                            entities.push_back(c);
                        }
                    }
                }
                if(a->name == "WinterIcon" && b->name == "bullet"){
                    if(isCollide(a,b)){ //if a WinterIcon collides with a bullet set both lives to false
                        a->life = false;
                        b->life = false;
                        asteroid::asteroidsfrozen = true; //set asteroid freeze static var and Winter wait effect static var to true
                        WinterIcon::WinterWait = true;
                        winter_start = std::chrono::system_clock::now(); // set an instantiation timer temp variable
                        Entity *e = new Entity(); //new explosion at the collision position
                        e->settings(sExplosion, a->x, b->y);
                        e->name = "explosion";
                        entities.push_back(e);

                    }
                }
                if(a->name == "WinterIcon" && b->name == "player" && !WinterIcon::WinterWait){
                    if(isCollide(a,b)){ //if collision between WinterIcon and player do same as if it was with a bullet, but don't set player life to false
                        a->life = false;
                        asteroid::asteroidsfrozen = true;
                        WinterIcon::WinterWait = true;
                        winter_start = std::chrono::system_clock::now();
                        Entity *e = new Entity();
                        e->settings(sExplosion, a->x, b->y);
                        e->name = "explosion";
                        entities.push_back(e);
                    }
                }
                if(a->name == "NuclearIcon" && b->name == "bullet"){
                    if(isCollide(a, b)){ //if collision between NuclearIcon and bullet, set both lives to false
                        a->life = false;
                        b->life = false;
                        Entity *e = new Entity; //instantiate an explosion
                        e->settings(sExplosion, a->x, b->y);
                        e->name = "explosion";
                        entities.push_back(e);
                        for(int i = 0; i < rand() % 200 + 100; i++){ //instantiate 100 - 300 bullets at the center with a random angle
                            bullet *b = new bullet;
                            b->settings(sBullet, W/2, H/2, rand() % 360);
                            entities.push_back(b);
                        }
                    }
                }
                if(a->name == "NuclearIcon" && b->name == "player"){
                    if(isCollide(a,b)){ //same as collision with NuclearIcon and player but don't set player life to false
                        a->life = false;
                        Entity *e = new Entity;
                        e->settings(sExplosion, a->x, b->y);
                        e->name = "explosion";
                        entities.push_back(e);
                        for(int i = 0; i < rand() % 200 + 100; i++){
                            bullet *b = new bullet;
                            b->settings(sBullet, W/2, H/2, rand() % 360);
                            entities.push_back(b);
                        }
                    }
                }
                if(a->name == "Cow" && b->name == "asteroid"){
                    if(isCollide(a, b)){ //if the Cow object collides with an asteroid set asteroid life to false
                        b->life = false;
                        Entity *e = new Entity; //instantiate an explosion object
                        e->name = "explosion";
                        e->settings(sExplosion, a->x, b->y);
                        entities.push_back(e);
                    }
                }
            }

        //choose stationary of moving animation based on if thrust is true or false
        if (p->thrust)  p->anim = sPlayer_go;
        else   p->anim = sPlayer;

        //check if explosion's animation are done, and if they are set life to zero
        for(auto e:entities)
            if (e->name=="explosion")
                if (e->anim.isEnd()) e->life=0;

        //if all asteroids are destroyed, create 15 asteroids each with a random position
        if(asteroid::countAsteroids == 0){
            for(int i=0;i<15;i++)
            {
                asteroid *a = new asteroid();
                a->settings(sRock, rand()%W, rand()%H, rand()%360, 25);
                entities.push_back(a); //add to entities list
            }
        }
        //if no other UFOs exist, there is a 1/500 chance of a UFO spawning
        if(rand() % 500 == 1 && UFO::get_number_UFOs() == 0){
            if(numufos > 0 && !musicplaying){ //if music isn't playing and ufo exists play the music
                music.play();
                musicplaying = true;
            }
            UFO *u = new UFO(); //instantiate a UFO object with a random y position and x of 0
            u->settings(sUFO, 0, rand() % H, 0, 32);
            entities.push_back(u);
        }
        //if no other WinterIcons exist there is a 1/400 chance of a WinterIcon spawning
        if(rand() % 400 == 1 && WinterIcon::countWinter == 0){
            WinterIconCreate = std::chrono::system_clock::now(); //set creation timer to now
            Icon *w = new WinterIcon; //instantiate Wintericon with random position and angle
            w->settings(sFreezeIcon, 1, rand() % H, rand() % 360, 30);
            entities.push_back(w);
        }
        //if no other NuclearIcons exist there is a 1/500 chance of a NuclearIcon spawning
        if(rand() % 500 == 1 && NuclearIcon::NuclearCount == 0){
            NuclearIconCreate = std::chrono::system_clock::now(); //set creation timer to now
            Icon *n = new NuclearIcon; //instantiate NuclearIcon with random position and angle
            n->settings(sNuclearIcon, rand() % W, 1, rand() % 360, 25);
            entities.push_back(n);
        }
        else{
            numufos++; //idk purpose of this
        }

        //check if 10 seconds has elapsed since the winter icon was hit
        if(std::chrono::duration_cast<std::chrono::seconds> (std::chrono::system_clock::now() - winter_start).count() == 10 && WinterIcon::WinterWait){
            asteroid::asteroidsfrozen = false; //set asteroidfrozen and wintereffect variables to false to indicate end of WinterIcon's effect
            WinterIcon::WinterWait = false;
        }
        //iterate through entities vector
        for(auto i=entities.begin();i!=entities.end();)
        {
            Entity *e = *i;

            //The WinterIcon exists for 7 seconds on screen and if not destroyed will self-implode
            if(std::chrono::duration_cast<std::chrono::seconds> (std::chrono::system_clock::now() - WinterIconCreate).count() > 7 && WinterIcon::countWinter > 0 && e->name == "WinterIcon"){
                e->life = false;
                Entity *ex = new Entity(); //instantiate a new explosion at the WinterIcon's last position
                ex->settings(sExplosion, e->x, e->y);
                ex->name = "explosion";
                entities.push_back(ex);

            }
            //The NuclearIcon exists for 7 seconds on-screen and if not destroyed will self-implode
            if(std::chrono::duration_cast<std::chrono::seconds> (std::chrono::system_clock::now() - NuclearIconCreate).count() > 7 && e->name == "NuclearIcon"){
                e->life = false;
                Entity *nucex = new Entity; //instantiate a new explosion at the NuclearIcon's last position
                nucex->settings(sExplosion, e->x, e->y);
                nucex->name = "explosion";
                entities.push_back(nucex);
            }

            e->update(); //update all entities and derived
            e->anim.update(); //update animations of all elements
            if(e->name == "UFO" && e->life == false){
                music.stop();
                musicplaying = false;
            }

            if (e->life==false) {i=entities.erase(i); delete e;}//if life is flase, erase the element and returns iterator to value after deleted value
            else i++; //increments if nothing is deleted
        }



        //////draw//////
        app.draw(background);
        for(auto i:entities) {//calls draw on all objects in entities vector
            i->draw(app);
        }
        app.display();
    }

    return 0;
}
