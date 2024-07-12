import pygame
import random
import sys
import math

# Initialize Pygame
pygame.init()

# Constants
WIDTH, HEIGHT = 500, 600
ROWS, COLS = 6, 5
CELL_SIZE = WIDTH // COLS
CAT_RADIUS = CELL_SIZE // 4
SUSHI_RADIUS = CAT_RADIUS // 2
GRAVITY = 1
WHITE = (255, 255, 255)
BLACK = (0, 0, 0)
RED = (255, 0, 0)
GREEN = (0, 255, 0)

# Set up the display
screen = pygame.display.set_mode((WIDTH, HEIGHT))
pygame.display.set_caption("Sushi Cat")

# Sushi locations
sushi_positions = [(random.randint(0, COLS-1) * CELL_SIZE + CELL_SIZE // 2,
                    random.randint(0, ROWS-1) * CELL_SIZE + CELL_SIZE // 2) for _ in range(15)]

# Obstacles (angled rectangles)
obstacles = []
for col in range(COLS):
    for row in range(ROWS):
        if random.random() < 0.15:
            angle = random.choice([-1, 1]) * random.randint(10, 30)  # Angle in degrees
            x = col * CELL_SIZE
            y = row * CELL_SIZE + CELL_SIZE // 2  # Start from middle of cell vertically
            width = CELL_SIZE
            height = CELL_SIZE // 3
            obstacles.append((x, y, width, height, angle))

# Game loop
running = True
cat_x, cat_y = None, 0
velocity_y = 0
velocity_x = 0
sushi_eaten = 0
is_squished = False
squish_timer = 0
mouse_held_down = False

while running:
    screen.fill(WHITE)
    
    # Draw sushi
    for sushi_x, sushi_y in sushi_positions:
        pygame.draw.circle(screen, GREEN, (sushi_x, sushi_y), SUSHI_RADIUS)
    
    # Draw obstacles
    for x, y, width, height, angle in obstacles:
        pygame.draw.rect(screen, RED, pygame.Rect(x, y, width, height), border_radius=5)
    
    # Handle mouse events
    for event in pygame.event.get():
        if event.type == pygame.QUIT:
            running = False
        elif event.type == pygame.MOUSEBUTTONDOWN:
            if event.button == 1:  # Left click
                mouse_x, mouse_y = pygame.mouse.get_pos()
                cat_x, cat_y = mouse_x, mouse_y
                mouse_held_down = True
        elif event.type == pygame.MOUSEBUTTONUP:
            if event.button == 1:  # Left click release
                mouse_held_down = False
                velocity_y = GRAVITY
                velocity_x = 0
    
    # Update the cat's position to follow the mouse while the button is held down
    if mouse_held_down:
        cat_x, cat_y = pygame.mouse.get_pos()
        cat_y = max(cat_y, 0)  # Prevent the cat from going above the screen
    
    # Cat falling logic
    if cat_x is not None and not mouse_held_down:
        cat_y += velocity_y
        cat_x += velocity_x
        velocity_y += GRAVITY
    
    # Collect sushi
    if cat_x is not None:
        cat_shape = pygame.Rect(int(cat_x - CAT_RADIUS), int(cat_y - CAT_RADIUS), 2 * CAT_RADIUS, 2 * CAT_RADIUS)
        sushi_positions = [s for s in sushi_positions if not cat_shape.collidepoint(s[0], s[1])]
        sushi_eaten = 15 - len(sushi_positions)
    
    # Check for collision with the bottom of the window or obstacles
    if cat_x is not None and not mouse_held_down:
        if cat_y > HEIGHT - CAT_RADIUS:
            cat_y = HEIGHT - CAT_RADIUS
            velocity_y = 0
            velocity_x = 0
            is_squished = True
            squish_timer = 10

        for x, y, width, height, angle in obstacles:
            obstacle_rect = pygame.Rect(x, y, width, height)
            if obstacle_rect.colliderect(pygame.Rect(cat_x - CAT_RADIUS, cat_y - CAT_RADIUS, 2 * CAT_RADIUS, 2 * CAT_RADIUS)):
                cat_y = y - CAT_RADIUS
                velocity_y = 0
                slide_direction = math.copysign(1, angle)
                velocity_x = slide_direction * abs(math.tan(math.radians(angle))) * GRAVITY
                is_squished = True
                squish_timer = 10
    
    # Draw the cat
    if cat_x is not None:
        if is_squished and squish_timer > 0:
            squish_timer -= 1
            pygame.draw.ellipse(screen, BLACK, (int(cat_x - CAT_RADIUS * 1.5), int(cat_y - CAT_RADIUS / 2), CAT_RADIUS * 3, CAT_RADIUS))
        else:
            is_squished = False
            pygame.draw.circle(screen, BLACK, (int(cat_x), int(cat_y)), CAT_RADIUS)

    # Display the score
    font = pygame.font.Font(None, 36)
    text = font.render(f"Sushi Eaten: {sushi_eaten}", 1, BLACK)
    screen.blit(text, (10, 10))

    pygame.display.flip()
    pygame.time.Clock().tick(60)

pygame.quit()
sys.exit()
