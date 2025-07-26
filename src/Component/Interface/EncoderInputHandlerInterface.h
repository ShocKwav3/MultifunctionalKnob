class EncoderInputHandlerInterface {
public:
    virtual ~EncoderInputHandlerInterface() = default;

    virtual void handleRotate(int delta) = 0;
    virtual void handleShortClick() = 0;
    virtual void handleLongClick() = 0;
};
