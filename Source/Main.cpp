#include <JuceHeader.h>
#include "MainComponent.h"

class OscillunaApplication : public juce::JUCEApplication {
public:
    OscillunaApplication() {}

    /**Получение названия приложения*/
    const juce::String getApplicationName() override {
        return ProjectInfo::projectName;
    }
    
    /**Получение версии приложения*/
    const juce::String getApplicationVersion() override {
        return ProjectInfo::versionString;
    }
    
    /**Разрешено ли иметь более одного экземпляра приложения*/
    bool moreThanOneInstanceAllowed() override {
        return true;
    }

    /**Инициализация окна**/
    void initialise(const juce::String& commandLine) override {
        mainWindow.reset(new MainWindow(getApplicationName()));
    }

    /**Закрытие окна**/
    void shutdown() override {
        mainWindow = nullptr; // Удаление окна
    }
    
    /**Вызывается, когда приложение получает запрос на завершение работы*/
    void systemRequestedQuit() override {
        quit();
    }

    /**Поведение при запуске ещё одного экземпляра приложения**/
    void anotherInstanceStarted (const juce::String& commandLine) override {
    }

    /**Этот класс реализует окно рабочего стола, которое содержит экземпляр класса MainComponent.*/
    class MainWindow : public juce::DocumentWindow {
    public:
        MainWindow(juce::String name) : DocumentWindow(name, // Название приложения
                                                       juce::Desktop::getInstance().getDefaultLookAndFeel().
                                                       findColour(juce::ResizableWindow::backgroundColourId), // Цвет окна
                                                       DocumentWindow::allButtons) { // Отображать все кнопки (закрытие, сворачивани и минимизация)
            setUsingNativeTitleBar(true); // Используем системный Title Bar
            setContentOwned(new MainComponent(), true); // Делаем MainComponent главным
            setResizable(true, true); // Разрешаем изменять размер
            // setResizeLimits(<#int newMinimumWidth#>, <#int newMinimumHeight#>, <#int newMaximumWidth#>, <#int newMaximumHeight#>) // Ограничения в изменения размера
            centreWithSize(getWidth(), getHeight()); // Размещаем компонент в центре
            setVisible(true); // Делаем видимым
        }

        /**Поведение в случае нажатия на кнопку закрытия приложения*/
        void closeButtonPressed() override {
            JUCEApplication::getInstance()->systemRequestedQuit();
        }

    private:
        JUCE_DECLARE_NON_COPYABLE_WITH_LEAK_DETECTOR(MainWindow) // Удаляем конструктор копирования и устанавливет детектор утечек.
    };

private:
    std::unique_ptr<MainWindow> mainWindow;
};

// Этот макрос генерирует процедуру main(), которая запускает приложение.
START_JUCE_APPLICATION(OscillunaApplication)
