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
        if (px == 2 && py < 6) return 1;
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
        if (py == 6 || py == 3 || py == 0) return 1;
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
        if ((py == 0 || py == 6) && px < 4) return 1;
        if (px == 4 && py > 0 && py < 6) return 1;
        return 0;
    }
    
    // Digits 0-9
    int digit = character - 10;
    
    // 0
    if (digit == 0) {
        if ((px == 0 || px == 4) && py > 0 && py < 6) return 1;
        if ((py == 0 || py == 6) && px > 0 && px < 4) return 1;
        return 0;
    }
    // 1
    if (digit == 1) {
        if (px == 2) return 1;
        if (py == 0 && px >= 1 && px <= 3) return 1;
        return 0;
    }
    // 2
    if (digit == 2) {
        if (py == 6 || py == 3 || py == 0) return 1;
        if (px == 4 && py > 3) return 1;
        if (px == 0 && py < 3) return 1;
        return 0;
    }
    // 3
    if (digit == 3) {
        if (py == 6 || py == 3 || py == 0) return 1;
        if (px == 4 && py != 0 && py != 3 && py != 6) return 1;
        return 0;
    }
    // 4
    if (digit == 4) {
        if (px == 0 && py >= 3) return 1;
        if (px == 4) return 1;
        if (py == 3) return 1;
        return 0;
    }
    // 5
    if (digit == 5) {
        if (py == 6 || py == 3 || py == 0) return 1;
        if (px == 0 && py > 3) return 1;
        if (px == 4 && py < 3) return 1;
        return 0;
    }
    // 6
    if (digit == 6) {
        if (py == 6 || py == 3 || py == 0) return 1;
        if (px == 0) return 1;
        if (px == 4 && py < 3) return 1;
        return 0;
    }
    // 7
    if (digit == 7) {
        if (py == 6) return 1;
        if (px == 4) return 1;
        return 0;
    }
    // 8
    if (digit == 8) {
        if (py == 6 || py == 3 || py == 0) return 1;
        if (px == 0 || px == 4) return 1;
        return 0;
    }
    // 9
    if (digit == 9) {
        if (py == 6 || py == 3 || py == 0) return 1;
        if (px == 4) return 1;
        if (px == 0 && py >= 3) return 1;
        return 0;
    }
    
    return 0;
}

void main() {
    float x = gl_FragCoord.x;
    float y = gl_FragCoord.y;
    
    // Draw "THE END" when game is over
    if (isGameOver == 1) {
        float centerX = width / 2.0;
        float centerY = height / 2.0;
        float charSize = 15.0; // Size of each character
        float spacing = 3.0;
        float totalWidth = 7.0 * charSize + 6.0 * spacing; // 7 characters + 6 spaces
        float startX = centerX - totalWidth / 2.0;
        
        // Check if we're in the text area
        if (y >= centerY - 7.0 * charSize / 2.0 && y <= centerY + 7.0 * charSize / 2.0) {
            float relY = y - (centerY - 7.0 * charSize / 2.0);
            int py = 6 - int(relY / charSize);
            
            if (py >= 0 && py <= 6) {
                // THE END = T(0) H(1) E(2) space N(3) E(2) N(3) D(4)
                int chars[7] = int[7](0, 1, 2, -1, 2, 3, 4); // -1 for space
                
                for (int i = 0; i < 7; i++) {
                    float charX = startX + float(i) * (charSize + spacing);
                    if (x >= charX && x < charX + 5.0 * charSize) {
                        float relX = x - charX;
                        int px = int(relX / charSize);
                        if (px >= 0 && px <= 4 && chars[i] >= 0) {
                            if (getPixel(chars[i], px, py) == 1) {
                                out_Color = vec4(1.0, 1.0, 1.0, 1.0); // White text
                                return;
                            }
                        }
                    }
                }
            }
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
    
    // Draw time display (top left)
    float timeY = height - 4.0 * size;
    float timeX = xOffset;
    int seconds = int(gameTime);
    
    // Draw time digits
    if (y >= timeY && y <= timeY + 7.0 * 3.0 && x >= timeX && x <= timeX + 15.0 * 3.0) {
        float relY = y - timeY;
        int py = 6 - int(relY / 3.0);
        
        if (py >= 0 && py <= 6) {
            // Display seconds as 3 digits
            int digit1 = (seconds / 100) % 10;
            int digit2 = (seconds / 10) % 10;
            int digit3 = seconds % 10;
            
            for (int i = 0; i < 3; i++) {
                float digitX = timeX + float(i) * 18.0; // 5 pixels * 3 scale + 3 spacing
                if (x >= digitX && x < digitX + 15.0) {
                    float relX = x - digitX;
                    int px = int(relX / 3.0);
                    if (px >= 0 && px <= 4) {
                        int d = (i == 0) ? digit1 : (i == 1) ? digit2 : digit3;
                        if (getPixel(d + 10, px, py) == 1) {
                            out_Color = vec4(1.0, 1.0, 1.0, 0.9);
                            return;
                        }
                    }
                }
            }
        }
    }
    
    // Draw distance display (top right)
    float distY = height - 4.0 * size;
    float distX = width - xOffset - 45.0; // 3 digits * 15
    int miles = int(distance);
    
    if (y >= distY && y <= distY + 7.0 * 3.0 && x >= distX && x <= width - xOffset) {
        float relY = y - distY;
        int py = 6 - int(relY / 3.0);
        
        if (py >= 0 && py <= 6) {
            int digit1 = (miles / 100) % 10;
            int digit2 = (miles / 10) % 10;
            int digit3 = miles % 10;
            
            for (int i = 0; i < 3; i++) {
                float digitX = distX + float(i) * 18.0;
                if (x >= digitX && x < digitX + 15.0) {
                    float relX = x - digitX;
                    int px = int(relX / 3.0);
                    if (px >= 0 && px <= 4) {
                        int d = (i == 0) ? digit1 : (i == 1) ? digit2 : digit3;
                        if (getPixel(d + 10, px, py) == 1) {
                            out_Color = vec4(1.0, 1.0, 0.8, 0.9); // Yellow for distance
                            return;
                        }
                    }
                }
            }
        }
    }
    
    discard;
}