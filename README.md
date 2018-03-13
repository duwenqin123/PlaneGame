# PlaneGame
### 游戏界面
![image](https://github.com/duwenqin123/PlaneGame/blob/master/image/%E5%88%9D%E5%A7%8B%E7%95%8C%E9%9D%A2.png)
![image](https://github.com/duwenqin123/PlaneGame/blob/master/image/%E6%B8%B8%E6%88%8F%E7%95%8C%E9%9D%A2.png)
### 游戏功能描述
* 界面采用windows实现
* 多种功能不同的敌机
* 增强子弹和补血、炸弹bonus
* 当飞机重生时，会有15秒的“无敌模式”，表现为一闪一闪的状态
* 游戏界面循环变动，代码如下：
```cpp
//先在mdc中贴上背景图，并让背景循环移动
	SelectObject(g_bufdc, g_hBackGround);
	static int nYOffset = 0;
	nYOffset = (nYOffset + 3) % WINDOW_HEIGHT;//3决定了背景移动的速度
	for (int i = 0; i < 1; i++){
		for (int j = -1; j < 1; j++){
			BitBlt(g_mdc, i*WINDOW_WIDTH, j*WINDOW_HEIGHT + nYOffset, WINDOW_WIDTH, WINDOW_HEIGHT, g_bufdc, 0, 0, SRCCOPY);
		}
	}
```
