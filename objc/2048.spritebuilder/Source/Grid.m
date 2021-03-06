//
//  Grid.m
//  2048
//
//  Created by Jianjun on 21/5/14.
//  Copyright (c) 2014 Apportable. All rights reserved.
//

#import "Tile.h"
#import "Grid.h"

// a 4x4 grid
static const int GRID_SIZE = 4;
static const CCTime SWIPE_DUR = 0.2;

@implementation Grid
{
    // nil means empty
    Tile* _grid[GRID_SIZE][GRID_SIZE];
}

-(void) didLoadFromCCB
{
    [self setupGestures];
    [self setupBackground];
    [self loadATileWithDuration:SWIPE_DUR];
}

// add swipe gestures for node Grid
-(void)setupGestures
{
    UISwipeGestureRecognizer* swl = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(swipeLeft)];
    swl.direction = UISwipeGestureRecognizerDirectionLeft;
    [[[CCDirector sharedDirector] view] addGestureRecognizer:swl];
    
    UISwipeGestureRecognizer* swr = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(swipeRight)];
    swr.direction = UISwipeGestureRecognizerDirectionRight;
    [[[CCDirector sharedDirector] view] addGestureRecognizer:swr];
    
    
    UISwipeGestureRecognizer* swu = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(swipeUp)];
    swu.direction = UISwipeGestureRecognizerDirectionUp;
    [[[CCDirector sharedDirector] view] addGestureRecognizer:swu];
    
    UISwipeGestureRecognizer* swd = [[UISwipeGestureRecognizer alloc] initWithTarget:self action:@selector(swipeDown)];
    swd.direction = UISwipeGestureRecognizerDirectionDown;
    [[[CCDirector sharedDirector]view]addGestureRecognizer:swd];
}

-(void)setupBackground
{
    // get width and height
    CCNode* bg = [CCBReader load:@"TileBg"];
    // calculate margin
    CGFloat mh = (self.contentSize.width - bg.contentSize.width * GRID_SIZE) / (GRID_SIZE + 1);
    CGFloat mv = (self.contentSize.height - bg.contentSize.height * GRID_SIZE) / (GRID_SIZE + 1);
    
    // set grid bg
    CGFloat x = mh;
    for (int i = 0; i < GRID_SIZE; ++i) {
        CGFloat y = mv;
        for (int j = 0; j < GRID_SIZE; ++j) {
            bg = [CCBReader load:@"TileBg"];
            [bg setPosition:ccp(x, y)];
            [self addChild:bg];
            y += bg.contentSize.height + mv;
        }
        x += bg.contentSize.width + mh;
    }
}

-(CGPoint) convertPositionFromIndex:(Index*)index
{
    // prepare
    static CCNode* tile;
    if (tile == nil) {
        tile = [CCBReader load:@"Tile"];
    }
    CGFloat w = tile.contentSize.width;
    CGFloat h = tile.contentSize.height;
    
    CGFloat mh = (self.contentSize.width - w * GRID_SIZE) / (GRID_SIZE + 1);
    CGFloat mv = (self.contentSize.height - h * GRID_SIZE) / (GRID_SIZE + 1);

    return ccp(mh * (index.x + 1) + w * index.x , mv * (index.y  + 1) + h * index.y);
}

// return NO if is full, or YES
// after calling this, loaded tile will be set to correct position
-(BOOL)loadATileWithDuration:(CCTime)d
{
    // used to generate random empty position
    Index* m[GRID_SIZE*GRID_SIZE];
    int n = 0;
    
    for (int i = 0; i < GRID_SIZE; ++i) {
        for (int j = 0; j < GRID_SIZE; ++j) {
            if (_grid[i][j] == nil) {
                m[n++] = [[Index alloc]initWithX:i andY:j];
            }
        }
    }
    if (n == 0) {
        return NO;
    }
    
    // empty found
    Index* ix = m[arc4random() % n];
    Tile* tile = [Tile loadWithIndex:ix];
    
    // fill _grid
    _grid[ix.x][ix.y] = tile;
    [tile setPosition:[self convertPositionFromIndex:ix]];
    [tile setScale:0];
    
    CCActionScaleTo* scaleTo = [CCActionScaleTo actionWithDuration:d scale:1];
    [tile runAction:scaleTo];
    
    [self addChild:tile];
    return YES;
}

// finally, b will be removed, and a will be updated
// return YES if a and b are not the same one and merge successfully, otherwise NO.
-(BOOL) mergeTileA:(Tile*)a andB:(Tile*)b toIndex:(Index*)ix withDuration:(CCTime)d
{
    if (a && b && ix && a.value == b.value) {
        BOOL hasNewEvent = a.index.x != b.index.x || a.index.y != b.index.y;
        CGPoint dest = [self convertPositionFromIndex:ix];

        CCActionMoveTo *moveTo = [CCActionMoveTo actionWithDuration:d position:dest];
        CCActionCallBlock *update = [CCActionCallBlock actionWithBlock:^(){
            [a setValue:a.value * 2];
            CCLOG(@"dest: (%f, %f)", dest.x, dest.y);
            CCLOG(@"a pos: %f, %f", a.position.x, a.position.y);
        }];
        CCActionSequence *seq = [CCActionSequence actionWithArray:@[moveTo, update]];
        [a runAction:seq];
        
        // !! important, action can't be used by multiple CCNode !!
        moveTo = [CCActionMoveTo actionWithDuration:d position:dest];
        CCActionRemove* rm = [CCActionRemove action];
        seq = [CCActionSequence actionWithArray:@[moveTo, rm]];
        [b runAction:seq];
        
        _grid[a.index.x][a.index.y] = nil;
        _grid[b.index.x][b.index.y] = nil;
        
        [a setIndex:ix];
        _grid[ix.x][ix.y] = a;
        CCLOG(@"x: %d, y: %d", ix.x, ix.y);
        return hasNewEvent;
    }
    return NO;
}

// return YES if t is moved, otherwise NO.
-(BOOL) moveTile:(Tile*)t toIndex:(Index*)ix withDuration:(CCTime)d
{
    CGPoint dest = [self convertPositionFromIndex:ix];

    if (t && ix) {
        BOOL hasMoved = t.index.x != ix.x || t.index.y != ix.y;
        CCActionSequence *moveTo = [CCActionMoveTo actionWithDuration:d position:dest];
        [t runAction:moveTo];
        
        _grid[t.index.x][t.index.y] = nil;
        
        [t setIndex:ix];
        _grid[ix.x][ix.y] = t;
        return hasMoved;
    }
    return NO;
}

-(void) swipeLeft
{
    BOOL shouldLoadATile = NO;
    for (int j = 0; j < GRID_SIZE; j++) {
        int p = 0;
        for (int i = 0; i < GRID_SIZE;) {
            Tile* t = _grid[i][j], *s = nil;
            // empty node, skip
            if (t == nil) {
                i++;
                continue;
            }
            // find next sibling
            int k = i + 1;
            for (; k < GRID_SIZE; k++) {
                s = _grid[k][j];
                if (s != nil) {
                    break;
                }
            }
            Index* pj = [[Index alloc] initWithX:p andY:j];
            CCLOG(@"p: %d, j: %d", p, j);
            if (s && s.value == t.value) {
                shouldLoadATile |= [self mergeTileA:t andB:s toIndex:pj withDuration:SWIPE_DUR];
                i = k + 1;
            } else {
                shouldLoadATile |= [self moveTile:t toIndex:pj withDuration:SWIPE_DUR];
                i = k;
            }
            p++;
        }
    }
    if (shouldLoadATile) {
        [self loadATileWithDuration:SWIPE_DUR];
    }
}

-(void) swipeRight
{
    BOOL shouldLoadATile = NO;
    for (int j = 0; j < GRID_SIZE; j++) {
        int p = GRID_SIZE-1;
        for (int i = GRID_SIZE-1; i >= 0;) {
            Tile* t = _grid[i][j], *s = nil;
            // empty node, skip
            if (t == nil) {
                i--;
                continue;
            }
            // find next sibling
            int k = i - 1;
            for (; k >= 0; k--) {
                s = _grid[k][j];
                if (s != nil) {
                    break;
                }
            }
            Index* pj = [[Index alloc] initWithX:p andY:j];
            if (s && s.value == t.value) {
                shouldLoadATile |= [self mergeTileA:t andB:s toIndex:pj withDuration:SWIPE_DUR];
                i = k - 1;
            } else {
                shouldLoadATile |= [self moveTile:t toIndex:pj withDuration:SWIPE_DUR];
                i = k;
            }
            p--;
        }
    }
    if (shouldLoadATile) {
        [self loadATileWithDuration:SWIPE_DUR];
    }
}

-(void) swipeUp
{
    BOOL shouldLoadATile = NO;
    for (int i = 0; i < GRID_SIZE; i++) {
        int p = GRID_SIZE-1;
        for (int j = GRID_SIZE-1; j >= 0;) {
            Tile* t = _grid[i][j], *s = nil;
            // empty node, skip
            if (t == nil) {
                j--;
                continue;
            }
            // find next sibling
            int k = j - 1;
            for (; k >= 0; k--) {
                s = _grid[i][k];
                if (s != nil) {
                    break;
                }
            }
            Index* ip = [[Index alloc] initWithX:i andY:p];
            if (s && s.value == t.value) {
                shouldLoadATile |= [self mergeTileA:t andB:s toIndex:ip withDuration:SWIPE_DUR];
                j = k - 1;
            } else {
                shouldLoadATile |= [self moveTile:t toIndex:ip withDuration:SWIPE_DUR];
                j = k;
            }
            p--;
        }
    }
    if (shouldLoadATile) {
        [self loadATileWithDuration:SWIPE_DUR];
    }
}

-(void) swipeDown
{
    BOOL shouldLoadATile = NO;
    for (int i = 0; i < GRID_SIZE; i++) {
        int p = 0;
        for (int j = 0; j < GRID_SIZE;) {
            Tile* t = _grid[i][j], *s = nil;
            // empty node, skip
            if (t == nil) {
                j++;
                continue;
            }
            // find next sibling
            int k = j + 1;
            for (; k < GRID_SIZE; k++) {
                s = _grid[i][k];
                if (s != nil) {
                    break;
                }
            }
            Index* ip = [[Index alloc] initWithX:i andY:p];
            if (s && s.value == t.value) {
                shouldLoadATile |= [self mergeTileA:t andB:s toIndex:ip withDuration:SWIPE_DUR];
                j = k + 1;
            } else {
                shouldLoadATile |= [self moveTile:t toIndex:ip withDuration:SWIPE_DUR];
                j = k;
            }
            p++;
        }
    }
    if (shouldLoadATile) {
        [self loadATileWithDuration:SWIPE_DUR];
    }
}

@end
