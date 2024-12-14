#include <GL/freeglut.h>
#include <vector>
#include <cstdlib>
#include <ctime>
#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"
#include "/Users/aisyahputri/Documents/snow_projects/include/Camera.h"  // ヘッダーファイル
#include <math.h>
#include <iostream>

#define PI 3.14159265358979323846

using namespace glm;
using namespace std;

// cam
vec3 cam_eye = {0.0f, 0.0f, 10.0f};
vec3 cam_at = {0.0f, 0.0f, 0.0f};
vec3 cam_up = {0.0f, 1.0f, 0.0f};

// Enum to track current season
enum Season {
    AUTUMN,
    WINTER,
    SUMMER,
    SPRING
};

//WINTER
// Global season variable
Season currentSeason = WINTER;

// particle snow
struct Particle {
    vec3 position;
    vec3 velocity;
    vec3 color;
    GLuint textureID;  // Texture ID randomly selected
};

// Global variables for snow system
vector<Particle> snowParticles;
const int numParticles = 1000;  // Number of snow particles
camera cam;

GLfloat ambientLight[] = { 0.5, 0.5, 0.5, 1.0 };  // Bright white light
GLfloat diffuseLight[] = { 0.5, 0.5, 0.5, 1.0 };  // Diffuse light is white
GLfloat specularLight[] = { 1.0, 1.0, 1.0, 1.0 }; // Specular light is white

GLfloat matAmbient[] = { 1.2 ,1.2 , 1.2 , 1.0 };  // Bright white material
GLfloat matDiffuse[] = {  0.5, 0.5, 0.5, 1.0};  // Diffuse material is white
GLfloat matSpecular[] = {  0.5, 0.5, 0.5, 1.0 };  // Specular material is white

// TexturesID
GLuint treeTexture, groundTexture, backgroundTexture;
GLuint snowTextures[3];  // snow textures
GLuint texFront, texLeft, texRight;
GLuint currentTexture;

// Load textures
GLuint LoadTexture(const char *filename) {
    int width, height, channels;
    
    // Load image
    unsigned char* image = stbi_load(filename, &width, &height, &channels, STBI_rgb_alpha);
    
    if (!image) {
        std::cout << "Failed to load texture: " << filename << std::endl;
        return 0;
    }

    GLuint textureID;
    glGenTextures(1, &textureID);
    glBindTexture(GL_TEXTURE_2D, textureID);
    
    // Set texture parameters
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    
    // Load the texture with alpha
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, image);
    
    // Free the image data
    stbi_image_free(image);
    
    return textureID;
}

void DrawSnow() {
 // Only draw snow in winter season
    if (currentSeason == WINTER) {
        // Existing snow drawing code
        GLfloat light_blue_diffuse[] = { 1.0f, 1.7f, 1.0f, 1.0f };
        GLfloat light_blue_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f };
        GLfloat light_blue_specular[] = { 0.6f, 0.8f, 1.0f, 1.0f };

        glMaterialfv(GL_FRONT, GL_DIFFUSE, light_blue_diffuse);
        glMaterialfv(GL_FRONT, GL_AMBIENT, light_blue_ambient);
        glMaterialfv(GL_FRONT, GL_SPECULAR, light_blue_specular);

        glPushMatrix();
        for (int i = 0; i < snowParticles.size(); i++) {
            Particle& p = snowParticles[i];

            glEnable(GL_BLEND);
            glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

            glPushMatrix();
            glTranslatef(p.position.x, p.position.y, p.position.z);
            glColor4f(1.0f, 1.7f, 1.0f, 0.9f);

            GLUquadric* quad = gluNewQuadric();
            gluQuadricTexture(quad, GL_TRUE);
            gluSphere(quad, 0.1f, 10, 13);
            gluDeleteQuadric(quad);
            glPopMatrix();

            glDisable(GL_BLEND);
        }
        glPopMatrix();
    }
}

// Snow particle update
float timeCounter = 0.5f;  // Time counter for snow particle update
void UpdateSnow() {
    for (auto& p : snowParticles) {
        // Make it fall along the Z-axis at a random speed
        float fallSpeed = rand() % 5 + 1;  // Random speed between 1 and 5
        p.velocity.z = -fallSpeed;  // Fall along the Z-axis

        // Randomize the horizontal drift
        p.position += p.velocity;

        // 画面外に出たらリセット
        if (p.position.z < 0.0f) {
            p.position.z = rand() % 50 / 1.0f + 10.0f;  // Reposition in the range of 10.0 to 60.0
            p.position.x = (rand() % 200 / 100.0f - 1) * 20.0;  // Randomly position along the X-axis
            p.position.y = (rand() % 200 / 100.0f - 1) * 20.0;  // Randomly position along the Y-axis
        }
    }
}

// Load snow textures
void LoadSnowTextures() {
    snowTextures[0] = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/SnowSmall.bmp");
    snowTextures[1] = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/SnowMiddle.bmp");
    snowTextures[2] = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/SnowLarge.bmp");
}

// Update the lighting in the scene
void UpdateLighting() {
    // Ambient light: soft, white light that affects the whole scene
    GLfloat ambientLight[] = { 1.0, 1.0, 1.0, 1.0 };  // Ambient light color (white)
    glLightfv(GL_LIGHT0, GL_AMBIENT, ambientLight);

    // Diffuse light: white light that simulates light scattering
    GLfloat diffuseLight[] = { 1.0, 1.0, 1.0, 1.0 };  // Diffuse light color (white)
    glLightfv(GL_LIGHT0, GL_DIFFUSE, diffuseLight);
    GLfloat specularLight[] = { 0.0, 0.0, 0.0, 1.0 };  // Specular light color (none)
    glLightfv(GL_LIGHT0, GL_SPECULAR, specularLight);

    // Set the light position at a fixed location
    GLfloat lightPosition[] = { 0.0f, 0.0f, 10.0f, 1.0f };  // Position of the light source
    glLightfv(GL_LIGHT0, GL_POSITION, lightPosition);

    // Enable the light source
    glEnable(GL_LIGHT0);
    glEnable(GL_LIGHTING);
}

void UpdateTextureBasedOnCameraDirection() {
    // Get the forward direction of the camera
    vec3 forwardDirection = normalize(cam.forward);

    // Determine the current texture based on the camera direction
    if (forwardDirection.z > 0.5f) {
        // Example: The camera is facing forward
        currentTexture = groundTexture;
    } else if (forwardDirection.z < -0.5f) {
        // Example: The camera is facing backward
        currentTexture = backgroundTexture;
    } else {
        // Example: The camera is facing left or right
        currentTexture = treeTexture;
    }
}

//AUTUMN
// New texture variables for autumn
GLuint autumnGroundTexture;
GLuint autumnLeafTexture;

// Leaf particle structure
struct LeafParticle {
    glm::vec3 position;
    glm::vec3 velocity;
    float size;
   float rotation;
};

// Global variables for leaf system
std::vector<LeafParticle> autumnLeaves;
GLuint leafTextures[1];  // Multiple leaf texture variations
const int numLeafParticles = 500;  // Number of leaves

// Initialize leaf particles
void InitializeLeafParticles() {
    autumnLeaves.clear();
    for (int i = 0; i < numLeafParticles; i++) {
        LeafParticle leaf;
        
        // Random initial position (wide area above the scene)
        leaf.position = glm::vec3(
            (rand() % 200 / 100.0f - 1) * 30.0f,  // X spread
            (rand() % 200 / 100.0f - 1) * 30.0f,  // Y spread
            rand() % 50 / 1.0f + 30.0f  // Start high above the ground
        );
        
        // Slightly randomized falling velocity
        leaf.velocity = glm::vec3(
            (rand() % 100 / 500.0f - 0.1f),  // Slight horizontal drift
            (rand() % 100 / 500.0f - 0.1f),  // Random horizontal movement
            -0.05f - (rand() % 50 / 1000.0f)  // Falling speed with some variation
        );
        
        // Varrying leaf size
        leaf.size = 0.2f + (rand() % 100 / 1000.0f);

        // Rotation properties
        leaf.rotation = rand() % 360;  // Initial random rotation
        
        autumnLeaves.push_back(leaf);
    }
}

// Update leaf particles
void UpdateLeafParticles() {
    for (auto& leaf : autumnLeaves) {
        // Update position
        leaf.position += leaf.velocity;
        
        // Add slight rotation
        leaf.rotation += (rand() % 10 - 5);  // Random rotation
        
        // Reset leaf if it goes below ground
        if (leaf.position.z < -2.0f) {
            // Respawn at the top
            leaf.position.z = rand() % 50 / 1.0f + 30.0f;
            leaf.position.x = (rand() % 200 / 100.0f - 1) * 30.0f;
            leaf.position.y = (rand() % 200 / 100.0f - 1) * 30.0f;
            
            // Reset velocity
            leaf.velocity = glm::vec3(
                (rand() % 100 / 500.0f - 0.1f),
                (rand() % 100 / 500.0f - 0.1f),
                -0.05f - (rand() % 50 / 100.0f)
            );

            // Reset rotation
           leaf.rotation = rand() % 360;
        }
    }
}

// Draw leaf particles
void DrawLeafParticles() {
    // Only draw leaves in autumn season
    if (currentSeason == AUTUMN) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, autumnLeafTexture);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        for (const auto& leaf : autumnLeaves) {
            glPushMatrix();
            
            // Position and rotate leaf
            glTranslatef(leaf.position.x, leaf.position.y, leaf.position.z);
            glRotatef(leaf.rotation, 0.0f, 0.0f, 1.0f);
            
            // Draw leaf as a textured quad
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-leaf.size, -leaf.size, 0.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f( leaf.size, -leaf.size, 0.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex3f( leaf.size,  leaf.size, 0.0f);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-leaf.size,  leaf.size, 0.0f);
            glEnd();
            glPopMatrix();
        }
        
        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
    }
}

// Load leaf textures
void LoadLeafTextures() {
    leafTextures[0] = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/maple_leaf.png");
    autumnLeafTexture = leafTextures[0];  // Default to red leaf texture
}

//SUMMER
GLuint summerGroundTexture;
GLuint summerTreeTexture;

struct RainParticle {
    glm::vec3 position;
    glm::vec3 velocity;
    float size;
};

// Global variables for rain system
std::vector<RainParticle> rainParticles;
const int numRainParticles = 1000;  // Number of rain drops

void InitializeRainParticles() {
    rainParticles.clear();
    for (int i = 0; i < numRainParticles; i++) {
        RainParticle rain;
        
        // Random initial position (wide area above the scene)
        rain.position = glm::vec3(
            (rand() % 200 / 100.0f - 1) * 30.0f,  // X spread
            (rand() % 200 / 100.0f - 1) * 30.0f,  // Y spread
            rand() % 50 / 1.0f + 30.0f  // Start high above the ground
        );
        
        // Falling velocity (faster and more vertical than leaves)
        rain.velocity = glm::vec3(
            (rand() % 100 / 500.0f - 0.1f),  // Slight horizontal drift
            (rand() % 100 / 500.0f - 0.1f),  // Random horizontal movement
            -1.0f - (rand() % 50 / 100.0f)  // Faster falling speed
        );
        
        // Varying raindrop sizes
        rain.size = 0.05f + (rand() % 100 / 1000.0f);
        
        rainParticles.push_back(rain);
    }
}

void UpdateRainParticles() {
    for (auto& rain : rainParticles) {
        // Update position
        rain.position += rain.velocity;
        
        // Reset rain if it goes below ground
        if (rain.position.z < -2.0f) {
            // Respawn at the top
            rain.position.z = rand() % 50 / 1.0f + 30.0f;
            rain.position.x = (rand() % 200 / 100.0f - 1) * 30.0f;
            rain.position.y = (rand() % 200 / 100.0f - 1) * 30.0f;
            
            // Reset velocity
            rain.velocity = glm::vec3(
                (rand() % 100 / 500.0f - 0.1f),
                (rand() % 100 / 500.0f - 0.1f),
                -1.0f - (rand() % 50 / 100.0f)
            );
        }
    }
}

void DrawRainParticles() {
    // Only draw rain in summer season
    if (currentSeason == SUMMER) {
        glDisable(GL_LIGHTING);
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        for (const auto& rain : rainParticles) {
            glPushMatrix();
            
            // Position and draw rain drop
            glTranslatef(rain.position.x, rain.position.y, rain.position.z);
            
            glColor4f(0.7f, 0.7f, 1.0f, 0.8f);  // Slight blue tint for rain
            
            // Draw rain as a thin cylinder (representing a rain drop)
            GLUquadric* quad = gluNewQuadric();
            gluCylinder(quad, rain.size * 0.5f, rain.size * 0.5f, 0.5f, 4, 1);
            gluDeleteQuadric(quad);
            
            glPopMatrix();
        }
        
        glDisable(GL_BLEND);
        glEnable(GL_LIGHTING);
    }
}

//SPRING
GLuint springGroundTexture;
GLuint springFlowerTexture;

struct FlowerParticle {
    glm::vec3 position;
    glm::vec3 velocity;
    float size;
    float rotation;  // Add rotation for more natural falling
};

std::vector<FlowerParticle> flowerParticles;
const int numFlowerParticles = 500; // Number of flower particles
GLuint flowerTextures[1];  // Flower texture

void InitializeFlowerParticles() {
    flowerParticles.clear();
    for (int i = 0; i < numFlowerParticles; i++) {
        FlowerParticle flower;
        
        // Random initial position (wide area above the scene)
        flower.position = glm::vec3(
            (rand() % 200 / 100.0f - 1) * 30.0f,  // X spread
            (rand() % 200 / 100.0f - 1) * 30.0f,  // Y spread
            rand() % 50 / 1.0f + 30.0f  // Start high above the ground
        );
        
        // Falling velocity with slight swaying motion
        flower.velocity = glm::vec3(
            (rand() % 100 / 500.0f - 0.1f),  // Slight horizontal drift
            (rand() % 100 / 500.0f - 0.1f),  // Random horizontal movement
            -0.05f - (rand() % 50 / 100.0f)  // Slower falling speed
        );
        
        // Varying flower sizes
        flower.size = 0.2f + (rand() % 100 / 1000.0f);
        
        // Random initial rotation
        flower.rotation = rand() % 360;
        
        flowerParticles.push_back(flower);
    }
}

void UpdateFlowerParticles() {
    for (auto& flower : flowerParticles) {
        // Update position
        flower.position += flower.velocity;
        
        // Add slight rotation
        flower.rotation += (rand() % 10 - 5);  // Random rotation
        
        // Reset flower if it goes below ground
        if (flower.position.z < -2.0f) {
            // Respawn at the top
            flower.position.z = rand() % 50 / 1.0f + 30.0f;
            flower.position.x = (rand() % 200 / 100.0f - 1) * 30.0f;
            flower.position.y = (rand() % 200 / 100.0f - 1) * 30.0f;
            
            // Reset velocity
            flower.velocity = glm::vec3(
                (rand() % 100 / 500.0f - 0.1f),
                (rand() % 100 / 500.0f - 0.1f),
                -0.05f - (rand() % 50 / 100.0f)
            );
            
            // Reset rotation
            flower.rotation = rand() % 360;
        }
    }
}

void DrawFlowerParticles() {
    // Only draw flowers in spring season
    if (currentSeason == SPRING) {
        glEnable(GL_TEXTURE_2D);
        glBindTexture(GL_TEXTURE_2D, springFlowerTexture);
        
        glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
        
        for (const auto& flower : flowerParticles) {
            glPushMatrix();
            
            // Position and rotate flower
            glTranslatef(flower.position.x, flower.position.y, flower.position.z);
            glRotatef(flower.rotation, 0.0f, 0.0f, 1.0f);
            
            // Draw flower as a textured quad
            glBegin(GL_QUADS);
            glTexCoord2f(0.0f, 0.0f); glVertex3f(-flower.size, -flower.size, 0.0f);
            glTexCoord2f(1.0f, 0.0f); glVertex3f(flower.size, -flower.size, 0.0f);
            glTexCoord2f(1.0f, 1.0f); glVertex3f(flower.size, flower.size, 0.0f);
            glTexCoord2f(0.0f, 1.0f); glVertex3f(-flower.size, flower.size, 0.0f);
            glEnd();
            glPopMatrix();
        }
        
        glDisable(GL_BLEND);
        glDisable(GL_TEXTURE_2D);
        
    }
}

void LoadFlowerTextures() {
    flowerTextures[0] = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/flower_pink.png");
    springFlowerTexture = flowerTextures[0]; 
}


// Draw ground
void DrawGround() {
    glDisable(GL_LIGHTING);
    glEnable(GL_TEXTURE_2D);

    // Select the appropriate ground texture based on current season
    GLuint currentGroundTexture;
    switch(currentSeason) {
        case WINTER:
            currentGroundTexture = groundTexture;
            break;
        case AUTUMN:
            currentGroundTexture = autumnGroundTexture;
            break;
        case SUMMER:
            currentGroundTexture = summerGroundTexture;
            break;
        case SPRING:
            currentGroundTexture = springGroundTexture;
            break;
        default:
            currentGroundTexture = groundTexture;  // Default to winter
    }
    
    glBindTexture(GL_TEXTURE_2D, currentGroundTexture);
    
    // Autumn ground color (more orange/brown)
    GLfloat autumnColor[] = {0.7f, 0.4f, 0.2f, 0.8f};
    GLfloat winterColor[] = {1.0f, 1.0f, 1.0f, 0.0f};

    glColor4fv((currentSeason == AUTUMN) ? autumnColor : winterColor);

    glBegin(GL_QUADS);
        glTexCoord2f(0.0f, 0.0f); glVertex3f(-20.0f, -20.0f, 0.0f);
        glTexCoord2f(1.0f, 0.0f); glVertex3f(20.0f, -20.0f, 0.0f);
        glTexCoord2f(1.0f, 1.0f); glVertex3f(20.0f, 20.0f, 0.0f);
        glTexCoord2f(0.0f, 1.0f); glVertex3f(-20.0f, 20.0f, 0.0f);
    glEnd();
    glPopMatrix();

    glDisable(GL_TEXTURE_2D);
    glDisable(GL_TEXTURE_2D);
}

void drawBoxWithMultipleTextures(GLuint texFront, GLuint texLeft, GLuint texRight) {
     // Left side
    if (texFront == 0 || texLeft == 0 || texRight == 0) {
        printf("One or more textures are not initialized properly.\n");
    }

    GLfloat default_diffuse[] = { 1.0f, 1.f, 1.0f, 1.0f }; // Default diffuse reflection (whitish)
    GLfloat default_ambient[] = { 1.0f, 1.0f, 1.0f, 1.0f }; // Default ambient reflection (light grey)
    GLfloat default_specular[] = { 0.0f, 0.3f, 0.0f, 1.0f }; // Default specular reflection (disabled)

    glMaterialfv(GL_FRONT, GL_DIFFUSE, default_diffuse);  // Reset diffuse reflection
    glMaterialfv(GL_FRONT, GL_AMBIENT, default_ambient);  // Reset ambient reflection
    glMaterialfv(GL_FRONT, GL_SPECULAR, default_specular); // Reset specular reflection

    // Draw the box with multiple textures
    float scale = 10.0f;
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-2.0f * scale, -2.0f * scale, -2.0f * scale);
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-2.0f * scale, -2.0f * scale,  2.0f * scale);
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-2.0f * scale,  2.0f * scale,  2.0f * scale);
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-2.0f * scale,  2.0f * scale, -2.0f * scale);

    glEnable(GL_TEXTURE_2D);
    glEnable(GL_DEPTH_TEST);

    // Front side
    glBindTexture(GL_TEXTURE_2D, texFront);
    glBegin(GL_QUADS);
    // Front face
    glBindTexture(GL_TEXTURE_2D, texFront);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-20.0f, 20.0f, 20.0f);  // Bottom-left
    glTexCoord2f(1.0f, 0.0f); glVertex3f( 20.0f, 20.0f, 20.0f);  // Bottom-right
    glTexCoord2f(1.0f, 1.0f); glVertex3f( 20.0f, 20.0f, -20.0f); // Top-right
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-20.0f, 20.0f, -20.0f); // Top-left
    glEnd();

    // Right side
    glBindTexture(GL_TEXTURE_2D, texRight);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(20.0f, -20.0f, -20.0f); // Bottom-left
    glTexCoord2f(1.0f, 0.0f); glVertex3f(20.0f, -20.0f, 20.0f); // Bottom-right
    glTexCoord2f(1.0f, 1.0f); glVertex3f(20.0f, 20.0f, 20.0f); // Top-right
    glTexCoord2f(0.0f, 1.0f); glVertex3f(20.0f, 20.0f, -20.0f); // Top-left
    glEnd();

    // Left side
    glBindTexture(GL_TEXTURE_2D, texLeft);
    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 0.0f); glVertex3f(-20.0f, -20.0f, -20.0f); // Bottom-left
    glTexCoord2f(1.0f, 0.0f); glVertex3f(-20.0f, -20.0f, 20.0f); // Bottom-right
    glTexCoord2f(1.0f, 1.0f); glVertex3f(-20.0f, 20.0f, 20.0f); // Top-right
    glTexCoord2f(0.0f, 1.0f); glVertex3f(-20.0f, 20.0f, -20.0f); // Top-left
    glEnd();
}

// Draw skybox
GLuint treeTexture02;  // New tree texture ID
GLuint treeTexture03;  // New tree texture ID

void LoadTreeTexture() {
    treeTexture02 = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/Tree02.tga");
    treeTexture03 = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/maple_tree.tga");
}

void drawStar(int N) {
    GLfloat starColor[] = {1.0f, 1.0f, 0.0f, 1.0f}; // Set color to yellow (RGBA)

    // Set material properties
    glMaterialfv(GL_FRONT, GL_DIFFUSE, starColor);  // Set the star color (yellow)

    glBegin(GL_POLYGON);  // Begin drawing the star

    // Draw the star
    for (int i = -2; i < N-2; i++) {
        // Outer vertices
        double angleOuter = 2 * M_PI * i / N;  // Outer angle
        glVertex2d(cos(angleOuter) * 0.8, sin(angleOuter) * 0.8);  // Set the coordinate

        // Inner vertices
        double angleInner = 2 * M_PI * (i + 0.5) / N;  // Inner angle
        glVertex2d(cos(angleInner) * 0.5, sin(angleInner) * 0.5);  // Set the coordinate
    }

    glEnd();  // End drawing the star
}

void drawTrunk(GLfloat height, GLfloat radius)
{
    GLUquadric* quad = gluNewQuadric();
    gluCylinder(quad, radius, radius, height, 32, 32); // // Draw a cylindrical trunk
    gluDeleteQuadric(quad);
}

void drawSphere()
{
    // 球体の描画
    GLUquadric* quad = gluNewQuadric();
    gluSphere(quad, 0.5f, 32, 32); // Draw a sphere
    gluDeleteQuadric(quad);
}

// Draw pole and sphere
void drawPoleAndSphere() {
   glEnable(GL_TEXTURE_2D);
    glBindTexture(GL_TEXTURE_2D, groundTexture);
    // Set up the trunk (adjust position, thickness, and height)
    GLfloat trunkHeight = 10.0f;
    GLfloat trunkRadius = 0.1f;

    // Set the position of the trunk
    GLfloat trunkX = 10.0f; // Move to the right along the X-axis
    GLfloat trunkY = 0.0f; // Y-axis (no vertical adjustment)
    GLfloat trunkZ = 0.0f; // Z-axis (no depth adjustment)

    glDisable(GL_LIGHTING); // Disable lighting for the pole and sphere
    // Draw the trunk
    glPushMatrix();
    glTranslatef(trunkX, trunkY, trunkZ); // Set the position of the trunk
    glColor3f(0.8f,0.8f,0.8f);
    drawTrunk(trunkHeight, trunkRadius); // Draw the trunk
    glPopMatrix();

    // Draw the sphere
    glPushMatrix();
    glTranslatef(trunkX, trunkY, trunkZ + trunkHeight);  // Set the position of the sphere
    glColor3f(0.9f,0.6f,0.0f);
    drawSphere();  // Draw the sphere
    glPopMatrix();

    glEnable(GL_LIGHTING); // Enable lighting after drawing the pole and sphere
}

// Draw Christmas tree
void DrawChristmasTree(float x, float y, float height) {

    // Bottom cone
    glPushMatrix();
    glTranslatef(x, y, height / 3.0f);  // Set the position of the bottom cone
    glColor3f(0.0f, 0.5f, 0.0f);
    glutSolidCone(3.0f, height / 3.0f, 10, 10);
    glPopMatrix();

    // Middle cone
    glPushMatrix();
    glTranslatef(x, y, height / 1.8f);
    glColor3f(0.0f, 0.4f, 0.0f);
    glutSolidCone(2.5f, height / 3.0f, 10, 10);
    glPopMatrix();

    // Top cone
    glPushMatrix();
    glTranslatef(x, y, height / 1.3f);
    glColor3f(0.0f, 0.3f, 0.0f);
    glutSolidCone(2.0f, height / 3.0f, 10, 10);
    glPopMatrix();

    // Draw the trunk
    glDisable(GL_LIGHTING); // Disable lighting for the trunk

    glPushMatrix();
    glTranslatef(x, y, height / 3.0f - (height / 3.0f));  // Set the position of the trunk
    glColor3f(0.6f, 0.3f, 0.1f);  // Set the color of the trunk
    GLUquadric* quad = gluNewQuadric();
    gluQuadricDrawStyle(quad, GLU_FILL);

    // Draw the trunk
    gluCylinder(quad, 0.3f, 0.3f, height / 3.0f, 10, 10);
    gluDeleteQuadric(quad);
    glPopMatrix();

    // Draw the star
    glPushMatrix();
    glTranslatef(x, y, height / 0.9f);
    glColor3f(1.0f,1.0f, 0.0f);  // Set the color of the star
    glRotatef(90, 1.0f, 0.0f, 0.0f);  // Rotate the star
    glDisable(GL_DEPTH_TEST); 
    drawStar(8);  
    glEnable(GL_DEPTH_TEST); 
    glPopMatrix();  
    glEnable(GL_LIGHTING); 
}

void RenderScene() {
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glLoadIdentity();
    gluLookAt(cam.eye.x, cam.eye.y, cam.eye.z, cam.at.x, cam.at.y, cam.at.z, cam.up.x, cam.up.y, cam.up.z);

    UpdateTextureBasedOnCameraDirection();

    drawBoxWithMultipleTextures(texFront, texLeft, texRight);

    UpdateLighting(); // Update the lighting in the scene

    // Draw the ground
    DrawGround();

    // Draw the Christmas tree
    DrawChristmasTree(0.0f, 0.0f, 10.0f);

    drawPoleAndSphere();

    // Draw the skybox
    if (currentSeason == WINTER) {
        DrawSnow();
    }

    // Draw leaf particles
    if (currentSeason == AUTUMN) {
        DrawLeafParticles();
    }

    // Draw rain particles
    if (currentSeason == SUMMER) {
         DrawRainParticles();
    }

    // Draw flower particles
    if (currentSeason == SPRING) {
        DrawFlowerParticles();
    }

    glutSwapBuffers();
}

void MyReshape(int width, int height) {

    // Calculate aspect ratio
    float aspect = static_cast<float>(width) / static_cast<float>(height);

    // Reset the viewport
    glViewport(0, 0, width, height);

    // Set up the projection matrix
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity(); // Reset the projection matrix
    gluPerspective(70.0, aspect, 1.0, 100.0); // Set up a perspective projection matrix

    // Reset the modelview matrix
    glMatrixMode(GL_MODELVIEW);
}

// Timer function for animation
void MyTimer(int value) {
    for (auto& p : snowParticles) {
        // Snow continues falling in the Z-axis direction
        p.position.z += p.velocity.z;  // Fall in the Z-axis direction

        // If snow particle goes below the ground, reset its position
        if (p.position.z < 0.0f) {
            p.position.z = rand() % 50 / 1.0f + 10.0f;  // Randomize the height
            p.position.x = (rand() % 200 / 100.0f - 1) * 20.0;  // X-axis random position
            p.position.y = (rand() % 200 / 100.0f - 1) * 20.0;  // Y-axis random position

            // Randomize the velocity
            p.velocity.z = -0.1f * (rand() % 5 + 1);  // Randomize the falling speed
        }
    }

    UpdateLeafParticles();
    UpdateRainParticles(); 
    UpdateFlowerParticles();

    glutPostRedisplay();
    glutTimerFunc(16, MyTimer, 1);  // Set the timer function
}

void LoadBackgroundTextures() {
    if (currentSeason == WINTER) {
        texFront = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/snowGround.png");
        texLeft = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/snowGround.png");
        texRight = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/snowGround.png");
    }
    if (currentSeason == AUTUMN) {
        texFront = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/autumn_sky.jpg");
        texLeft = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/autumn_sky.jpg");
        texRight = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/autumn_sky.jpg");
    }
    if (currentSeason == SUMMER) {
        texFront = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/summer_sky.jpg");
        texLeft = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/summer_sky.jpg");
        texRight = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/summer_sky.jpg");
    }
    if (currentSeason == SPRING) {
        texFront = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/spring_sky.jpg");
        texLeft = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/spring_sky.jpg");
        texRight = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/spring_sky.jpg");
    }
}

void initializeTextures() {
    // Winter textures
    treeTexture = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/tree.jpg");
    groundTexture = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/snowGround.png");

    // Autumn textures
    autumnGroundTexture = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/autumn_sky.jpg");
    autumnLeafTexture = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/tree.jpg");

    // Summer textures
    summerGroundTexture = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/ground.bmp");
    summerTreeTexture = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/tree.jpg");

    //Spring textures
    springGroundTexture = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/lawn.bmp");
    springFlowerTexture = LoadTexture("/Users/aisyahputri/Documents/snow_projects/Data/tree.jpg");

    // Validate that all textures are loaded correctly
    if (treeTexture == 0 || groundTexture == 0 || 
        autumnGroundTexture == 0 || autumnLeafTexture == 0 ||
        summerGroundTexture == 0 || summerTreeTexture == 0 ||
        springGroundTexture == 0 || springFlowerTexture == 0) {
        printf("Error: One or more textures failed to load\n");
        exit(1);  // Exit the program if textures fail to load
    }
}

void KeyboardHandler(unsigned char key, int x, int y) {
    switch (key) {
        case 'w':
        case 'W':
            currentSeason = WINTER;
            LoadBackgroundTextures();
            LoadSnowTextures();
            break;
        case 'a':
        case 'A':
            currentSeason = AUTUMN;
            LoadBackgroundTextures();
            LoadLeafTextures();
            InitializeLeafParticles();
            break;
        case 's':
        case 'S':
            currentSeason = SUMMER;
            LoadBackgroundTextures();
            InitializeRainParticles();
            break;
        case 'p':
        case 'P':
            currentSeason = SPRING;
            LoadBackgroundTextures();
            LoadFlowerTextures();
            InitializeFlowerParticles();
            break;
    }
    glutPostRedisplay();
}

// int main
int main(int argc, char** argv) {
    srand(static_cast<unsigned>(time(0)));

    cam.InitCamera(vec3(0.0f, -20.0f, 15.0f), vec3(0.0f, 0.0f, 10.0f), vec3(0.0f, 0.0f, 1.0f));

    glutInit(&argc, argv);
    glutInitDisplayMode(GLUT_DOUBLE | GLUT_RGB | GLUT_DEPTH);
    glutInitWindowSize(800, 600);
    glutCreateWindow("Seasonal Change");

    glEnable(GL_DEPTH_TEST);
    glClearColor(1.0f, 1.0f, 1.0f, 1.0f);  // Set the background color to white

    initializeTextures();

    // Add keyboard handler
    glutKeyboardFunc(KeyboardHandler);

    LoadBackgroundTextures();

    // Initialize snow particles
    for (int i = 0; i < numParticles; i++) {
        Particle p;
        p.position = vec3((rand() % 200 / 100.0f - 1) * 20.0, (rand() % 200 / 100.0f - 1) * 20.0, rand() % 50 / 1.0f + 10.0);
        p.velocity = vec3(0.0f, 0.0f, -0.1f);
        snowParticles.push_back(p);
    }

    glutDisplayFunc(RenderScene);
    glutReshapeFunc(MyReshape);
    glutTimerFunc(30, MyTimer, 1);
    glutMainLoop();

    return 0;
}