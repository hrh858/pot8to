clang++ \
-std=c++11 -Wall -Wextra -g \
-o pot8to main_macos.mm platform_macos.mm \
-framework Cocoa \
-framework UniformTypeIdentifiers
