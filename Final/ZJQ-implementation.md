# [Camera Roaming 摄像机漫游](https://learnopengl-cn.github.io/01%20Getting%20started/09%20Camera/)
- `glm::lookAt` 创建一观察矩阵
- `glfwSetCursorPosCallback` 获取鼠标位置调整摄像机方向
- `glfwGetKey` 获取键盘输入移动摄像机位置。
- 第一人称
- 物体碰撞检测

# [Sky Box 天空盒](https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/06%20Cubemaps/)
- http://www.custommapmakers.org/skyboxes.php 提供了很多天空盒，选择海底天空盒
- `glGenTextures`, `glBindTexture`, `glTexImage2D`, `glTexParameteri`
- 需要一组新的着色器
- 需要把天空盒缩放到足够大
- 指定一个足够视距的投影矩阵
- 先渲染天空盒，再渲染其他物体，并且在渲染天空盒时禁用深度写入

# [Display Text 显示文字，中文/英文/立体/平面](https://learnopengl-cn.github.io/06%20In%20Practice/02%20Text%20Rendering/)
- [FreeType](https://www.freetype.org/) 字体库为字形生成静态位图（作为纹理）及度量值（定位）
- 或 Windows 的 [`wglUseFontBitmaps`](https://blog.csdn.net/wxwtj/article/details/6620032) 显示文字，[`wglUseFontOutlines`](https://blog.csdn.net/augusdi/article/details/20572533) 显示立体文字
- 操作方法
- 立体路标

# [Anti-Aliasing 抗锯齿](https://learnopengl-cn.github.io/04%20Advanced%20OpenGL/11%20Anti%20Aliasing/)
- 离屏多重采样抗锯齿(Multisample Anti-aliasing, MSAA)
- 使用 `glTexImage2DMultisample` 替代 `glTexImage2D` 多重采样纹理附件
- 使用 `glRenderbufferStorageMultisample` 替代 `glRenderbufferStorage` 多重采样渲染缓冲对象
- `glBlitFramebuffer` 渲染到多重采样帧缓冲