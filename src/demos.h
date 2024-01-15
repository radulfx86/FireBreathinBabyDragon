#ifndef DEMOS_H
#define DEMOS_H
#include "raylib.h"
#include <string>
#include <vector>

void showAnimationDemo();

void showMotionDemo();

void showTileDemo();

void showLifeDemo();

void showAudioDemo();

void showGame();

/**
 * @brief selection menu storage and management use "addEntry" to create new entries.
 */
class DemoList
{
public:
    const static int MARGIN_X = 10;
    const static int MARGIN_Y = 5;
    const static int TEXT_MARGIN_Y = 5;
    const static int BUTTON_HEIGHT = 40;
    /// @brief function pointer type - easier to type this way
    using DemoFunPtr_t = void (*)();
    /// @brief demo menu entry structure
    using DemoEntry_t = struct { Rectangle dimensions; std::string text; DemoFunPtr_t function; };
    DemoList(Rectangle dimensions) : dimensions(dimensions) {}
    ~DemoList() = default;

    /**
     * @brief add menu entry
     * 
     * @param text      test to show on button
     * @param function  function pointer to "main" function of demo
     */
    void addEntry(std::string text, DemoFunPtr_t function);

    /**
     * @brief check if a menu point was selected.
     * @return function pointer assigned to selected entry
     */
    DemoFunPtr_t checkSelection();

    /**
     * @brief draw DemoList
     */
    void draw();

private:
    /// @brief  (display) dimensions of menu
    Rectangle dimensions;
    /// @brief  menu entries
    std::vector<DemoEntry_t> menuEntries;

};

#endif // DEMOS_H