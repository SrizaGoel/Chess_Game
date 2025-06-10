#include <iostream>
#include <conio.h>
// For getch() and _kbhit() – used in snake game input
#include <vector>
#include <cstdlib>
#include <ctime>
#include <algorithm>
#include <windows.h>
// For Sleep()
#include <stdlib.h>
#include <cstring>
#include <fstream>
// For file handling (high score)

using namespace std;

//Snakegame

class Game
{
public:
    static int score;
    static int highScore;
    int height_of_board=10;
    int width_of_board=15;
    vector<int> snake_tail_x, snake_tail_y;
    int snake_len_tail=0;
    int food_col, food_row;
    int head_x, head_y;
    char direction='d';
    bool gameover=false;
    void initial();            // Initialize game variables
    void setup();              // Draw the board
    void generate_food();      // Generate food not on snake
    static void readHighScore();  // Read from file
    static void writeHighScore(); // Write to file
    void input();              // Handle user input
    void grid();               // Game logic – movement, collisions

};

int Game::score=0;
int Game::highScore = 0;

// Read high score from file
void Game::readHighScore()
{
    std::ifstream infile("highscore.txt");
    if (infile.is_open())
    {
        infile>>highScore;
        infile.close();
    }
    else
    {
        highScore = 0;
    }
}

// Method to write high score to file
void Game::writeHighScore()
{
    std::ofstream outfile("highscore.txt");
    if (outfile.is_open())
    {
        outfile<<highScore; // Write the high score to file
        outfile.close();
    }
}
// Generate food at a location not occupied by snake
void Game::generate_food()
{
    do
    {
        food_col=1+rand() % (width_of_board-2);
        food_row=1+rand() % (height_of_board-2);
    } while ((food_col==head_x && food_row==head_y) ||
             (find(snake_tail_x.begin(), snake_tail_x.end(), food_row) != snake_tail_x.end() &&
              find(snake_tail_y.begin(), snake_tail_y.end(), food_col) != snake_tail_y.end()));
}

// Set initial head position, read high score

void Game::initial()
{
    srand(time(0));
    head_x=width_of_board / 2;
    head_y=height_of_board / 2;
    generate_food();
    readHighScore();
}

void Game::setup()
{
    system("cls");

    // Top border
    for (int i=0; i < width_of_board; i++) cout<<"* ";
    cout<<"\n";

    for (int i=1; i < height_of_board-1; i++)
    {
        cout<<"*";
        for (int j=1; j < width_of_board-1; j++)
        {
            if (j==head_x && i==head_y)
            {
                cout<<"S ";
            }
            else if (j==food_col && i==food_row)
            {
                cout<<"F ";
            }
            else
            {
                bool printed=false;
                for (int k=0;k<snake_len_tail;k++)
                {
                    if (snake_tail_y[k]==i && snake_tail_x[k]==j)
                    {
                        cout<<"s ";
                        printed=true;
                        break;
                    }
                }
                if (!printed) cout<<"  ";
            }
        }
        cout<<"*\n";
    }

    // Bottom border
    for (int i=0; i < width_of_board; i++) cout<<"* ";
    cout<<"\nScore: "<<score<<endl;
}

void Game::input()
{
    if (_kbhit())
    {
        char ch=getch();
        switch (ch)
        {
            case 'w': if (direction != 's') direction='w'; break;
            case 's': if (direction != 'w') direction='s'; break;
            case 'a': if (direction != 'd') direction='a'; break;
            case 'd': if (direction != 'a') direction='d'; break;
            case 'x': gameover=true; break;
        }
    }
}

// Movement, food collision, wall/tail collision
void Game::grid()
{
    int head_x_original=head_x;
    int head_y_original=head_y;

    switch (direction)
    {
        case 'w': head_y--; break;
        case 's': head_y++; break;
        case 'a': head_x--; break;
        case 'd': head_x++; break;
    }

    // Collision with wall
    if (head_x==0||head_x==width_of_board-1||head_y==0||head_y==height_of_board-1)
    {
        gameover=true;
        return;
    }

    // Collision with tail
    for (int i=0;i<snake_len_tail;i++)
    {
        if (snake_tail_y[i]==head_y && snake_tail_x[i]==head_x)
        {
            gameover=true;
            return;
        }
    }

    // Eat food
    if (head_x==food_col && head_y==food_row)
    {
        snake_len_tail++;
        score += 15;
        snake_tail_x.push_back(head_x_original);
        snake_tail_y.push_back(head_y_original);
        generate_food();
    }

    // Move tail
    for (int i=snake_len_tail-1;i>0;i--)
    {
        snake_tail_x[i]=snake_tail_x[i-1];
        snake_tail_y[i]=snake_tail_y[i-1];
    }
    if (snake_len_tail>0)
    {
        snake_tail_x[0]=head_x_original;
        snake_tail_y[0]=head_y_original;
    }
}

//ChessGame
class Piece
{
protected:
    string colour;
public:
    Piece(const string &c)
    {
        colour=c;
    }
    virtual bool move_validity(int fromrow,int fromcol,int torow,int tocol,vector<vector<Piece*>>&board_piece)=0;
    virtual string get_symbol()=0;

    string get_colour()
    {
        return colour;
    }
    virtual ~Piece(){}
};

class Empty_Place:public Piece
{
public:
    Empty_Place():Piece(""){}
    string get_symbol()
    {
        return ". ";
    }
    bool move_validity(int fromrow,int fromcol,int torow,int tocol,vector<vector<Piece*>>&board_piece)
    {
        return false;
    }
};

//Pawn class

class Pawn:public Piece
{
public:
    Pawn(const string&c):Piece(c){}
    string get_symbol()
    {
        return (colour=="black")?"BP":"WP";
    }
    // Check pawn movement: forward or diagonal capture

    bool move_validity(int fromrow,int fromcol,int torow,int tocol,vector<vector<Piece*>>&board_piece)
    {
        int direction=(colour=="white"?1:-1); //black moves -1(comes down) and white moves +1(moves up)
        if(fromcol==tocol && torow-fromrow==direction && board_piece[torow][tocol]->get_colour()=="")
        {
            return true;
        }
        //diagonal move
        if(abs(tocol-fromcol)==1 && torow - fromrow == direction && board_piece[torow][tocol]->get_colour()!="" && board_piece[torow][tocol]->get_colour()!=colour)
        {
            return true;
        }
        return false;

    }
};

//Bishop Class

class Bishop:public Piece
{
public:
    Bishop(const string &c):Piece(c){}
    string get_symbol()
    {
        return (colour=="black")?"BB":"WB";
    }
    bool move_validity(int fromrow,int fromcol,int torow,int tocol,vector<vector<Piece*>>&board_piece)
    {
        if(abs(fromrow-torow)!=abs(fromcol-tocol))
        {
            return false;
        }

        // Determine direction of movement

        int row_dir=(-fromrow+torow)/(abs(fromrow-torow));
        int col_dir=(-fromcol+tocol)/(abs(fromcol-tocol));
        int move_r=row_dir+fromrow;
        int move_c=col_dir+fromcol;
        while(move_r!=torow && move_c!=tocol)
        {
            if(board_piece[move_r][move_c]->get_colour()!="")
                return false;
            move_r+=row_dir;
            move_c+=col_dir;
        }
        if(board_piece[torow][tocol]!=nullptr && board_piece[torow][tocol]->get_colour()== colour)
        {
            return false;
        }
        return true;
    }
};
//Knight Class

class Knight:public Piece
{
public:
    Knight(const string &c):Piece(c){}
    string get_symbol()
    {
        return (colour=="black")?"BN":"WN";
    }
    // Knight moves in L-shape

    bool move_validity(int fromrow,int fromcol,int torow,int tocol,vector<vector<Piece*>>&board_piece)
    {
        int r=abs(torow-fromrow);
        int c=abs(tocol-fromcol);
        if ((r==1 && c==2 )||(r==2 && c==1))
        {
            if(board_piece[torow][tocol]==nullptr || board_piece[torow][tocol]->get_colour()!=colour)
            {
                return true;
            }
        }
        return false;
    }
};

//Rook Class

class Rook : public Piece
{
public:
    Rook(const string& c):Piece(c){}
    string get_symbol()
    {
        return(colour=="black")?"BR":"WR";
    }

    // Rook moves straight in rows or columns
    bool move_validity(int fromrow,int fromcol,int torow,int tocol,vector<vector<Piece*>>& board_piece)
    {
        if (fromrow!=torow && fromcol!=tocol)
        {
            return false; // Not moving straight
        }

        int row_step=(torow-fromrow)==0?0:(torow - fromrow)/abs(torow - fromrow);
        int col_step=(tocol-fromcol)==0?0:(tocol - fromcol)/abs(tocol - fromcol);
        int r=fromrow + row_step;
        int c=fromcol + col_step;
        while (r != torow || c != tocol)
        {

            if (board_piece[r][c]->get_colour() != "")
            {
                return false;
            }
            r += row_step;
            c += col_step;
        }
        // Final destination check
        if (board_piece[torow][tocol]->get_colour() == "" || board_piece[torow][tocol]->get_colour() != colour)
        {
            return true;
        }

        return false;
    }

};

//Queen Class

class Queen:public Piece
{
    Rook rook;
    Bishop bishop;

public:
    Queen(const string &c):Piece(c),bishop(c),rook(c){}
    string get_symbol()
    {
        return (colour=="black")?"BQ":"WQ";
    }

    // Queen = Bishop + Rook movements
    bool move_validity(int fromrow,int fromcol,int torow,int tocol,vector<vector<Piece*>>&board_piece)
    {
        if(bishop.move_validity(fromrow,fromcol,torow,tocol,board_piece) || rook.move_validity(fromrow,fromcol,torow,tocol,board_piece))
        {
            return true;
        }
        return false;

    }
};

//King Class

class King:public Piece
{
public:
    King(const string &c):Piece(c){}
    string get_symbol()
    {
        return (colour=="black")?"BK":"WK";
    }

     // King moves one square in any direction
    bool move_validity(int fromrow,int fromcol,int torow,int tocol,vector<vector<Piece*>>&board_piece)
    {
        int row_move=abs(torow-fromrow);
        int col_move=abs(tocol-fromcol);
        if((row_move==0 && col_move==1)||(row_move==1 && col_move==0)||(row_move==1 && col_move==1))
        {
            if(board_piece[torow][tocol] && board_piece[torow][tocol]->get_colour()!=colour)
            {
                return true;
            }
            if(board_piece[torow][tocol]==nullptr)
            {
                return true;
            }
        }
        return false;
    }
};

//ChessBoard create

class ChessBoard
{
    vector<vector<Piece*>>board;
public:

    ChessBoard()
    {
        // Initialize board with nullptr
        for (int i=0;i<8;i++)
        {
            vector<Piece*>row;
            for(int j=0;j<8;j++)
            {
                row.push_back(nullptr);
            }
            board.push_back(row);
        }
        setupBoard();
    }

    ~ChessBoard()
    {
        // Cleanup memory
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; ++j)
            {
                delete board[i][j];
            }
        }
    }

    // Setup all pieces on initial chess board
    void setupBoard()
    {
        //Pawn
        for(int i=0;i<8;i++)
        {
            board[1][i]=new Pawn("white");
            board[6][i]=new Pawn("black");
        }
        //white
        board[0][0]=new Rook("white");
        board[0][7]=new Rook("white");
        board[0][1]=new Knight("white");
        board[0][6]=new Knight("white");
        board[0][2]=new Bishop("white");
        board[0][5]=new Bishop("white");
        board[0][3]=new Queen("white");
        board[0][4]=new King("white");

        //black
        board[7][0]=new Rook("black");
        board[7][7]=new Rook("black");
        board[7][1]=new Knight("black");
        board[7][6]=new Knight("black");
        board[7][2]=new Bishop("black");
        board[7][5]=new Bishop("black");
        board[7][3]=new Queen("black");
        board[7][4]=new King("black");

        // Empty Spaces
        for(int i=2;i<6;i++)
        {
            for(int j=0;j<8;j++)
            {
                board[i][j]=new Empty_Place();
            }
        }
    }

    void printBoard()
    {
        for (int i = 7; i >= 0; --i)
        {
            cout << i + 1 << "  ";
            for (int j = 0; j < 8; ++j)
            {
                cout << board[i][j]->get_symbol()<< " ";
            }
            cout << " " << endl;
        }
        cout << "   1  2  3  4  5  6  7  8\n";
    }
    bool movement(int fromrow,int fromcol,int torow,int tocol,const string&current_player)
    {
        Piece*from=board[fromrow][fromcol];
        Piece*to=board[torow][tocol];
        if(from==nullptr || (from->get_colour()!=current_player))
        {
            return false;
        }
        if(to && (to->get_colour()==current_player))
        {
            return false;
        }
        if(!from->move_validity(fromrow,fromcol,torow,tocol,board))
        {
            return false;
        }
        board[torow][tocol]=from;
        board[fromrow][fromcol]=new Empty_Place();
        return true;
    }
    bool is_Checkmate(const string&current_player)
    {
        int torow,tocol;
        string opponent_colour=(current_player=="black")?"white":"black";
        string opponent_king=(opponent_colour=="white")?"WK":"BK";
        for(int i=0;i<8;i++)
        {
            for(int j=0;j<8;j++)
            {
                if(this->board[i][j]->get_symbol()==opponent_king)
                {
                    torow=i;
                    tocol=j;
                    break;
                }
            }
        }

        bool in_check = false;
        for (int i = 0; i < 8; i++)
        {
            for (int j = 0; j < 8; j++)
            {
                Piece* piece = this->board[i][j];
                if (piece != nullptr && piece->get_colour() == current_player)
                {
                    if (piece->move_validity(i, j, torow, tocol, board))
                    {
                        in_check = true;
                        break;
                    }
                }
            }
        }
        if (!in_check)
        {
            return false; // Not in check, so not checkmate
        }
        return true;
    }
    bool is_King_Alive(const string &colour)
    {
        string symbol=(colour=="black")?"BK":"WK";
        for(int i=0;i<8;i++)
        {
            for(int j=0;j<8;j++)
            {
                if(board[i][j]->get_symbol()==symbol)
                {
                    return true;
                }
            }
        }
        cout<<endl<<symbol<<" captured ";
        string winner=(symbol=="WK")?"Black":"White";
        cout<<endl<<"Game over !";
        cout<<endl<<winner<<" Player wins the game !";
        return false;
    }
};
class Chess_Game
{
    ChessBoard chess;
    string currentplayer;
public:
    //starts game with white
    Chess_Game()
    {
        currentplayer="white";
    }
    void start()
    {
        while(true)
        {
            int from_row,to_row,from_col,to_col;
            chess.printBoard();
            cout<<endl<<"Enter "<<currentplayer<<"'s move (Which piece to move ?) : ";
            cin>>from_row>>from_col;
            cout<<endl<<"Enter the final position : ";
            cin>>to_row>>to_col;
            from_col--;from_row--;to_col--;to_row--;
            cout<<endl;
            if(from_row<0 || from_row>7 || from_col<0 || from_col>7 || to_row<0 || to_row>7 || to_col<0 || to_col>7)
            {
                cout<<endl<<"Invalid input (Coordinates out of bounds) !"<<endl<<endl;
                continue;
            }
            if(!chess.movement(from_row,from_col,to_row,to_col,currentplayer))
            {
                cout<<endl<<"Invalid move !"<<endl<<endl;
                continue;
            }
            if(chess.is_Checkmate(currentplayer))
            {
                cout<<endl<<" CHECKMATE ! "<<endl<<endl;
            }
            string opponent=(currentplayer=="white")?"black":"white";
            if (!chess.is_King_Alive(opponent))
            {
                break;
            }
            currentplayer=(currentplayer=="white")?"black":"white";
        }
    }
};
class Begin
{
public:
    void Border()
    {

        cout<<endl;
        cout<<"..........................................................................\n";

        cout<<"...............  .  .  .  .  .  .  .  .  .  .  .  .  .  .  ...............\n";

        cout<<"...............  .  .  .  .  .  .  .  .  .  .  .  .  .  .  ...............\n";

        cout<<"...............  .  .  .  .  .  .  .  .  .  .  .  .  .  .  ...............\n";

        cout<<"........................... SERPENT'S CHECKMATE ..........................\n";

        cout<<"...............  .  .  .  .  .  .  .  .  .  .  .  .  .  .  ...............\n";

        cout<<"...............  .  .  .  .  .  .  .  .  .  .  .  .  .  .  ...............\n";

        cout<<"...............  .  .  .  .  .  .  .  .  .  .  .  .  .  .  ...............\n";

        cout<<"..........................................................................\n";

        cout<<endl<<endl;

    }
    void Chess_Intro()
    {
        cout<<"==========================================================================="<<endl;

        cout<<endl<<endl<<"                   WELCOME TO CHESS GAME          "<<endl<<endl<<endl;

        cout<<"==========================================================================="<<endl<<endl;

        cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"<<endl;

        cout<<" Keys to symbols used : "<<endl<<endl<<endl;
        cout<<" . = white space"<<endl;
        cout<<" Blank space = black space"<<endl;
        cout<<" WP = White pawn"<<endl;
        cout<<" WN = White Knight"<<endl;
        cout<<" WB = White Bishop"<<endl;
        cout<<" WR = White Rook"<<endl;
        cout<<" WQ = White Queen"<<endl;
        cout<<" WK = White King"<<endl;
        cout<<" BP = Black pawn"<<endl;
        cout<<" BN = Black Knight"<<endl;
        cout<<" BB = Black Bishop"<<endl;
        cout<<" BR = Black Rook"<<endl;
        cout<<" BQ = Black Queen"<<endl;
        cout<<" BK = Black King"<<endl<<endl;

        cout<<"Rule for move is : "<<endl;

        cout<<"Move by selecting row & column to another valid location using row & column. "<<endl<<endl<<endl;

        cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"<<endl;
    }
    void Snake_Intro()
    {
        cout<<"==========================================================================="<<endl;

        cout<<endl<<endl<<"                   WELCOME TO SNAKE GAME          "<<endl<<endl<<endl;

        cout<<"==========================================================================="<<endl<<endl;

        cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"<<endl;

        cout<<" Keys to symbols used : "<<endl<<endl<<endl;
        cout<<" w = Move up"<<endl;
        cout<<" a = Move left"<<endl;
        cout<<" s = Move down"<<endl;
        cout<<" d = Move right"<<endl;
        cout<<"Rule for move is : "<<endl;

        cout<<"Eat food to grow, avoid walls and your tail. Use w a s d to move, X to exit. "<<endl<<endl<<endl;

        cout<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~\n"<<endl;
    }
};
int main()
{
    Begin b;b.Border();
    Game::readHighScore();
    int choice;
    cout<<"Choose your game to begin : "<<endl<<endl;
    cout<<"1. Chess Game"<<endl;
    cout<<"2. Snake Game"<<endl;
    cout<<"Enter your choice to begin : ";cin>>choice;
    if (choice==1)
    {
        cout<<"Launching Chess Game..."<<endl<<endl;
        b.Chess_Intro();
        Chess_Game g;
        g.start();
    }
    else if (choice==2)
    {
        cout<<"Launching Snake Game..."<<endl<<endl;
        b.Snake_Intro();
        //Sleep(10000);
        cout<<endl<<"Enter any key to begin : ";
        getch();
        Game g;
        g.initial();
        while (!g.gameover)
        {
            g.setup();
            g.input();
            g.grid();
            Sleep(1000); // 1000 ms delay for game pace
        }

        cout<<"\nGame Over!\nFinal Score: "<<Game::score<<"\n";
        if (Game::score > Game::highScore)
        {
            Game::highScore=Game::score; // Update high score
            Game::writeHighScore();
            // Write new high score to file
            cout<<"New High Score: "<<Game::highScore<<" !"<<endl;
        }
        else
        {
            cout<<"High Score: "<<Game::highScore<<endl;
            // Display existing high score
        }
    }
    else
    {
        cout << "Invalid choice. Exiting...\n";
    }
    return 0;
}

