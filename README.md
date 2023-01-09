# AGGL

Another Goddam Graphics Library

this is a WIP, for now just a collection of ideas

## Features (planned)

- Transparency (for Images via transparency key)
- For Screens with partial screen update capability
- Object Based System
- Minimal RAM required or Adjustable RAM Limit (compute one chunk of frame at a time and transmitt seperately)
- Use U8Glib Fonts
- Support N-Color Displays
- Dithering on Monochrome Displays
- Font Scaling, blowing up small fonts when flash is limited
- Custom Graphics Elements (derive default element and create your own e. g. text with dynamic border)

## Graphics Object List

### TEXT

- Show/Hide
- Alter Text
- Alter Position
- Alter Font
- Width Limit (NONE, WRAP, HIDE_OVERFLOW)

```c++
AGGL::textHandle myText = AGGL::createText(10, 20, "Hello World", u8g2_font_6x13_mf);
if(myText)
{
  myText->setLimit(AGGL:HIDE_OVERFLOW, 50)
  AGGL::update();

  myText->hide();
  AGGL::update();

  myText->move(20, 30);
  AGGL::update();

  myText->change("What's going on?");
  AGGL::update();
}

```

### LINE

- Show/Hide
- Alter Positions
- Alter Color
- Alter Width
- Alter Endcaps

```c++
AGGL::lineHandle myLine = AGGL::createLine(10, 20, 30, 40, 1);
if(myLine)
{
  AGGL::update();

  myLine->hide();
  AGGL::update();

  myLine->change(20, 30, 40, 50);
  AGGL::update();

  myLine->color(AGGL:RGB(255, 0, 0));
  AGGL::update();
}

```
