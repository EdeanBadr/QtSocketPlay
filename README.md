# **QtSocketPlay**

### **Overview**
QtSocketPlay is a real-time video streaming application built with Qt and FFmpeg. It supports multiple client connections and streams video efficiently using socket-based communication.

---

### **Features**
- **Real-time Streaming:** Streams video frames to multiple clients with low latency.
- **Dynamic Layout Management:** Automatically adjusts the UI layout based on the number of active streams.
- **Multithreaded Architecture:** Uses QThread for handling network communication and streaming tasks.
- **FFmpeg Integration:** Leverages FFmpeg for video decoding and processing.
- **Memory Leak Detection:** Built with AddressSanitizer to ensure efficient resource management.

---

### **Technologies Used**
- **Qt Framework**: For GUI and networking.
- **FFmpeg**: For multimedia processing.
- **C++17**: For core application logic.
- **AddressSanitizer**: For detecting memory issues.

---

### **Requirements**
- **Operating System:** Linux (tested on Ubuntu 22.04)
- **Libraries:**
  - Qt6 or Qt5
  - FFmpeg (libavutil, libavcodec, libavformat, etc.)
  - GCC or Clang with AddressSanitizer support
- **Build Tools:** CMake

---

### **Installation**
1. **Clone the Repository**
   ```bash
   git clone https://github.com/username/QtSocketPlay.git
   cd QtSocketPlay
   ```

2. **Install Dependencies**
   ```bash
   sudo apt update
   sudo apt install build-essential cmake qtbase5-dev libavcodec-dev libavformat-dev libavutil-dev
   ```

3. **Build the Project**
   ```bash
   mkdir build
   cd build
   cmake .. -DCMAKE_BUILD_TYPE=Debug
   make
   ```

4. **Run the Application**
   ```bash
   ./QTserver   # For server
   ./QTclient   # For client
   ```

---

### **Usage**
1. Start the server by running `QTserver`.
2. Launch multiple client instances (`QTclient`) to connect to the server.
3. Observe the dynamic UI layout update as more clients join.
4. Use the `AddressSanitizer` build to monitor for memory issues:
   ```bash
   ASAN_OPTIONS=detect_leaks=1 ./QTserver
   ```

---

## Demo Video
A demo video showcasing the creation of 4 streams and how the layout dynamically adapts as streams are added or removed is included in this repository.

![Description of GIF](screenCast.gif)

You can also play it locally:
```bash
mpv screenCast.mp4  # or use any media player
```

---

### **Contributing**
Contributions are welcome! Feel free to submit a pull request or open an issue for any bugs or feature requests.

---

### **License**
[MIT License](LICENSE)

---



