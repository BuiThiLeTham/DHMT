// ui.frag
#version 330 core
out vec4 out_Color;

uniform float health;
uniform float width;
uniform float height;
uniform float gameTime;
uniform float distance;
uniform int isGameOver;

// Simple bitmap font for digits and letters (5x7 pixels)
int getPixel(int character, int px, int py) {
    // T
    if (character == 0) {
        if (py == 6) return 1;
        if (px == 2) return 1;
        return 0;
    }
    // H
    if (character == 1) {
        if (px == 0 || px == 4) return 1;
        if (py == 3 && px >= 0 && px <= 4) return 1;
        return 0;
    }
    // E
    if (character == 2) {
        if (px == 0) return 1;
        if (py == 0 || py == 3 || py == 6) return 1;
        return 0;
    }
    // N
    if (character == 3) {
        if (px == 0 || px == 4) return 1;
        if (px == 1 && py >= 4) return 1;
        if (px == 2 && py == 3) return 1;
        if (px == 3 && py <= 2) return 1;
        return 0;
    }
    // D
    if (character == 4) {
        if (px == 0) return 1;
        if (py == 0 || py == 6) return 1;
        if (px == 4 && py > 0 && py < 6) return 1;
        return 0;
    }
    
    // Digits 0-9 (character ID = 10-19)
    // Standard 5x7 bitmap font
    int digit = character - 10;
    
    // 0 - Rectangle with hollow center
    if (digit == 0) {
        if ((px == 0 || px == 4) && (py >= 1 && py <= 5)) return 1;
        if ((py == 0 || py == 6) && (px >= 1 && px <= 3)) return 1;
        return 0;
    }
    
    // 1 - Vertical line with base
    if (digit == 1) {
        if (px == 2) return 1;
        if (py == 1 && px == 1) return 1;
        if (py == 6 && (px >= 1 && px <= 3)) return 1;
        return 0;
    }
    
    // 2 - S shape
    if (digit == 2) {
        if (py == 0 && (px >= 1 && px <= 4)) return 1;
        if (py == 1 && px == 4) return 1;
        if (py == 2 && px == 4) return 1;
        if (py == 3 && (px >= 1 && px <= 3)) return 1;
        if (py == 4 && px == 0) return 1;
        if (py == 5 && px == 0) return 1;
        if (py == 6 && (px >= 0 && px <= 4)) return 1;
        return 0;
    }
    
    // 3 - Two bumps on right
    if (digit == 3) {
        if (py == 0 && (px >= 0 && px <= 4)) return 1;
        if (py == 1 && px == 4) return 1;
        if (py == 2 && px == 4) return 1;
        if (py == 3 && (px >= 1 && px <= 4)) return 1;
        if (py == 4 && px == 4) return 1;
        if (py == 5 && px == 4) return 1;
        if (py == 6 && (px >= 0 && px <= 4)) return 1;
        return 0;
    }
    
    // 4 - Right angle with vertical
    if (digit == 4) {
        if (px == 0 && (py >= 0 && py <= 3)) return 1;
        if (py == 3 && (px >= 1 && px <= 4)) return 1;
        if (px == 4) return 1;
        return 0;
    }
    
    // 5 - Reverse S
    if (digit == 5) {
        if (py == 0 && (px >= 0 && px <= 4)) return 1;
        if (py == 1 && px == 0) return 1;
        if (py == 2 && px == 0) return 1;
        if (py == 3 && (px >= 0 && px <= 3)) return 1;
        if (py == 4 && px == 4) return 1;
        if (py == 5 && px == 4) return 1;
        if (py == 6 && (px >= 0 && px <= 4)) return 1;
        return 0;
    }
    
    // 6 - Circle on bottom
    if (digit == 6) {
        if (py == 0 && (px >= 1 && px <= 3)) return 1;
        if (py == 1 && px == 0) return 1;
        if (py == 2 && px == 0) return 1;
        if (py == 3 && (px >= 0 && px <= 3)) return 1;
        if (py == 4 && (px == 0 || px == 4)) return 1;
        if (py == 5 && (px == 0 || px == 4)) return 1;
        if (py == 6 && (px >= 1 && px <= 3)) return 1;
        return 0;
    }
    
    // 7 - Top line with diagonal
    if (digit == 7) {
        if (py == 0 && (px >= 0 && px <= 4)) return 1;
        if (py == 1 && px == 4) return 1;
        if (py == 2 && px == 3) return 1;
        if (py == 3 && px == 3) return 1;
        if (py == 4 && px == 2) return 1;
        if (py == 5 && px == 2) return 1;
        if (py == 6 && px == 2) return 1;
        return 0;
    }
    
    // 8 - Two stacked circles
    if (digit == 8) {
        if (py == 0 && (px >= 1 && px <= 3)) return 1;
        if (py == 1 && (px == 0 || px == 4)) return 1;
        if (py == 2 && (px == 0 || px == 4)) return 1;
        if (py == 3 && (px >= 1 && px <= 3)) return 1;
        if (py == 4 && (px == 0 || px == 4)) return 1;
        if (py == 5 && (px == 0 || px == 4)) return 1;
        if (py == 6 && (px >= 1 && px <= 3)) return 1;
        return 0;
    }
    
    // 9 - Circle on top
    if (digit == 9) {
        if (py == 0 && (px >= 1 && px <= 3)) return 1;
        if (py == 1 && (px == 0 || px == 4)) return 1;
        if (py == 2 && (px == 0 || px == 4)) return 1;
        if (py == 3 && (px >= 1 && px <= 4)) return 1;
        if (py == 4 && px == 4) return 1;
        if (py == 5 && px == 4) return 1;
        if (py == 6 && (px >= 1 && px <= 3)) return 1;
        return 0;
    }
    
    return 0;
}

void main() {
    float x = gl_FragCoord.x;
    float y = gl_FragCoord.y;
    
    // Draw red square when game is over
    if (isGameOver == 1) {
        float centerX = width / 2.0;
        float centerY = height / 2.0;
        float squareSize = 200.0; // Large red square
        
        // Draw filled red square in center
        if (x >= centerX - squareSize/2.0 && x <= centerX + squareSize/2.0 &&
            y >= centerY - squareSize/2.0 && y <= centerY + squareSize/2.0) {
            out_Color = vec4(1.0, 0.0, 0.0, 0.9); // Bright red
            return;
        }
        discard;
    }
    
    // Draw health bar
    float size = height / 40.0;
    float padding = size / 5.0;
    float interval = size + padding;
    float yOffset = height - 2.0 * size;
    float xOffset = 2.0 * size;
    float length = (health / 5.0) * interval;
    
    if (y >= yOffset && y <= yOffset + size && x >= xOffset && x <= xOffset + length) {
        float localX = x - xOffset;
        localX -= floor(localX / interval) * interval;
        if (localX <= size) {
            out_Color = vec4(220.0/255.0, 20.0/255.0, 60.0/255.0, 0.9); // Crimson red
            return;
        }
    }
    
    // Draw time display (CENTER TOP - large and visible)
    float timeScale = 8.0; // Larger size for visibility
    float timeY = height - 60.0; // Near top
    int seconds = int(gameTime);
    
    // Calculate center position for time
    float timeWidth = 3.0 * (5.0 * timeScale + 5.0); // 3 digits + spacing
    float timeX = (width - timeWidth) / 2.0; // Center horizontally
    
    // Draw time digits (large and centered)
    if (y >= timeY && y <= timeY + 7.0 * timeScale) {
        float relY = y - timeY;
        int py = 6 - int(relY / timeScale); // Invert for screen coordinates
        
        if (py >= 0 && py <= 6) {
            // Display seconds as 3 digits
            int digit1 = (seconds / 100) % 10;
            int digit2 = (seconds / 10) % 10;
            int digit3 = seconds % 10;
            
            for (int i = 0; i < 3; i++) {
                float digitX = timeX + float(i) * (5.0 * timeScale + 5.0);
                if (x >= digitX && x < digitX + 5.0 * timeScale) {
                    float relX = x - digitX;
                    int px = int(relX / timeScale);
                    if (px >= 0 && px <= 4) {
                        int d = (i == 0) ? digit1 : (i == 1) ? digit2 : digit3;
                        if (getPixel(d + 10, px, py) == 1) {
                            out_Color = vec4(1.0, 1.0, 0.0, 1.0); // Bright yellow
                            return;
                        }
                    }
                }
            }
        }
    }
    
    // Draw distance display (CENTER BOTTOM - large and visible)
    float distScale = 6.0; // Large size
    float distY = 60.0; // Near bottom
    int miles = int(distance);
    
    // Calculate center position for distance
    float distWidth = 3.0 * (5.0 * distScale + 5.0); // 3 digits + spacing
    float distX = (width - distWidth) / 2.0; // Center horizontally
    
    if (y >= distY && y <= distY + 7.0 * distScale) {
        float relY = y - distY;
        int py = 6 - int(relY / distScale); // Invert for screen coordinates
        
        if (py >= 0 && py <= 6) {
            int digit1 = (miles / 100) % 10;
            int digit2 = (miles / 10) % 10;
            int digit3 = miles % 10;
            
            for (int i = 0; i < 3; i++) {
                float digitX = distX + float(i) * (5.0 * distScale + 5.0);
                if (x >= digitX && x < digitX + 5.0 * distScale) {
                    float relX = x - digitX;
                    int px = int(relX / distScale);
                    if (px >= 0 && px <= 4) {
                        int d = (i == 0) ? digit1 : (i == 1) ? digit2 : digit3;
                        if (getPixel(d + 10, px, py) == 1) {
                            out_Color = vec4(0.0, 1.0, 1.0, 1.0); // Cyan for distance
                            return;
                        }
                    }
                }
            }
        }
    }
    
    discard;
}