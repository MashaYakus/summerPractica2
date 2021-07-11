#include <iostream>

using namespace std;

struct TCoordinate {
    int x;
    int y;

    TCoordinate& operator = (const TCoordinate &b) {
      x = b.x;
      y = b.y;
      return *this;
   }
};

bool operator == (const TCoordinate a, const TCoordinate b) {
    if (a.x == b.x && a.y == b.y)
        return true;
    return false;
}

void printTCoordinate (const TCoordinate a) {
    cout << "(" << a.x << ", " << a.y << ")" << endl;
}

template <class T>

class TVector {
    private:
        T *DataSave;
    public:
	    int Capacity;
        int Max;
	    T *Data;

    TVector(int n) {
	    Capacity = n;
	    Max = n * 2;
	    Data = new T[Max];
	    DataSave = 0;
    }
    TVector() {
		Capacity = 0;
	    Max = 0;
	    Data = 0;
	    DataSave = 0;
    }
   ~TVector() {
   		delete[] Data;
   		Data = 0;
	}
    void Push() {
        if(Data == 0) {
            Data = new T[1];
            Max = 1;
        }
        if(Capacity == Max) {
                DataSave = new T[Max * 2];
                Max = Max * 2;
                for(int i = 0; i < Capacity; ++i) {
                        DataSave[i] = Data[i];
                    }
                delete[] Data;
                Data = DataSave;
                DataSave = 0;
        }
        Capacity += 1;
    }
    void Add(struct TCoordinate a) {
        Push();
        Data[Capacity - 1].x = a.x;
        Data[Capacity - 1].y = a.y;
    }
    void Nul() {
    	for(int i = 0; i < Capacity; ++i) {
    		Data[i] = T();
        }
    }
    T& operator[] (const unsigned int &index) {
        return Data[index];
    }
};

enum Motion {Stop, Right, Left, Up, Down};
enum Complexity {Easy = 300, Medium = 150, Hard = 0};
struct TSnake {
    bool gameOver;
    TCoordinate head, fruit, pastTeil = {-1, -1};
    TVector <struct TCoordinate> tail;
    int score = 0;
    Motion state;
    Motion pause;
    Complexity game;
    bool walls;
};


