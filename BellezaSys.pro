QT += widgets network

CONFIG += c++17
TEMPLATE = app
TARGET = BellezaSys

INCLUDEPATH += include

SOURCES += \
    src/core/Agendamento.cpp \
    src/core/BellezaSystem.cpp \
    src/core/Chatbot.cpp \
    src/core/Financeiro.cpp \
    src/core/HandleBody.cpp \
    src/core/Profissional.cpp \
    src/core/Servico.cpp \
    src/core/Types.cpp \
    src/core/Usuario.cpp \
    src/gui/MainWindow.cpp \
    src/gui/OllamaLlmClient.cpp \
    src/gui/main.cpp

HEADERS += \
    include/bellezasys/core/AgendaService.hpp \
    include/bellezasys/core/Agendamento.hpp \
    include/bellezasys/core/AgendamentoImpl.hpp \
    include/bellezasys/core/BellezaSystem.hpp \
    include/bellezasys/core/BellezaSystemImpl.hpp \
    include/bellezasys/core/Chatbot.hpp \
    include/bellezasys/core/ChatbotImpl.hpp \
    include/bellezasys/core/LlmClient.hpp \
    include/bellezasys/core/Financeiro.hpp \
    include/bellezasys/core/FinanceiroImpl.hpp \
    include/bellezasys/core/HandleBody.hpp \
    include/bellezasys/core/Profissional.hpp \
    include/bellezasys/core/ProfissionalImpl.hpp \
    include/bellezasys/core/Servico.hpp \
    include/bellezasys/core/ServicoImpl.hpp \
    include/bellezasys/core/Types.hpp \
    include/bellezasys/core/Usuario.hpp \
    include/bellezasys/core/UsuarioImpl.hpp \
    src/gui/MainWindow.hpp \
    src/gui/OllamaLlmClient.hpp
