project(DrawGraphTest)

add_executable(${PROJECT_NAME} main.cpp)

target_link_libraries(${PROJECT_NAME} PRIVATE
    DrawGraph
    SciCalc::SciCalc
    MathRenderer::MathRenderer
)
