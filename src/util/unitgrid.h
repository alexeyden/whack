#ifndef UNITGRID_H
#define UNITGRID_H

#include <vector>

template<typename Item> class UnitGrid
{
public:
	UnitGrid(unsigned side) {
		_grid = new GridCell[side * side];
		_side = side;
	}
	
	~UnitGrid() {
		delete [] _grid;
	}
	
	void add(Item it, float ex, float ey, float size) {
		float hw = size/2.0f;
		int x0 = ex - hw, x1 = ex + hw;
		int y0 = ey - hw, y1 = ey + hw;
	
		for(int x = x0; x <= x1; x++) {
			for(int y = y0; y <= y1; y++) {
				_grid[x + y * _side].push_back(it);
			}
		}
		_list.push_back(it);
	}
	
	void remove(Item it, float ex, float ey, float size) {
		float hw = size/2.0f;
		int x0 = ex - hw, x1 = ex + hw;
		int y0 = ey - hw, y1 = ey + hw;
	
		for(int x = x0; x <= x1; x++) {
			for(int y = y0; y <= y1; y++) {
				auto& items = _grid[x + y * _side];
				
				auto found = std::find(items.begin(), items.end(), it);
				if(found != items.end())
					items.erase(found);
			}
		}
		
		_list.erase(std::find(_list.begin(), _list.end(), it));
	}
	
	void move(Item it, float x0, float y0,
						float x1, float y1, float size) {
		if(x0 != x1 || y0 != y1) {
			float hw = size/2.0f;
			
			int x0_prev = x0 - hw, x1_prev = x0 + hw;
			int y0_prev = y0 - hw, y1_prev = y0 + hw;
			
			int x0_now = x1 - hw, x1_now = x1 + hw;
			int y0_now = y1 - hw, y1_now = y1 + hw;
			
			for(int x = x0_prev; x <= x1_prev; x++) {
				for(int y = y0_prev; y <= y1_prev; y++) {
					auto& items = _grid[x + y * _side];
					
					auto found = std::find(items.begin(), items.end(), it);
					
					if(found != items.end())
						items.erase(found);
				}
			}
			for(int x = x0_now; x <= x1_now; x++) {
				for(int y = y0_now; y <= y1_now; y++) {
					_grid[x + y * _side].push_back(it);
				}
			}
		}
	}
	
	bool cellOccupied(int x, int y) const {
		return _grid[x + y * _side].size() > 0;
	}
	
	const std::vector<Item>& cellItems(int x, int y) const {
		return _grid[x + y * _side]; 
	}
	
	const std::vector<Item>& allItems() const {
		return _list;
	}
	
private:
	typedef std::vector<Item> GridCell;
	
	unsigned _side;
	GridCell* _grid;
	std::vector<Item> _list; 
};

#endif // UNITGRID_H
