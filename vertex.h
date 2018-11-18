#ifndef VERTEX_H
#define VERTEX_H
#include <QVector3D>
#include <QVector2D>

class Vertex
{
public:
  // Constructors
    Q_DECL_CONSTEXPR Vertex(){}

    Q_DECL_CONSTEXPR explicit Vertex(const QVector3D &position):m_position(position){}
    Q_DECL_CONSTEXPR explicit Vertex(const float x,const float y,const float z) : m_position(x,y,z){}

    Q_DECL_CONSTEXPR Vertex(const QVector3D &position, const QVector3D &color):m_position(position),m_color(color){}
    Q_DECL_CONSTEXPR Vertex(const float x,const float y,const float z ,
                            const float r,const float b,const float g,
                            const float x_n,const float y_n,const float z_n)
        :m_position(x,y,z),m_color(r,b,g),m_normal(x_n,y_n,z_n){}

    Q_DECL_CONSTEXPR Vertex(const QVector3D &position,const QVector2D &texture):
        m_position(position),m_texture(texture){}

    Q_DECL_CONSTEXPR Vertex(const float x,const float y,const float z ,
                            const float a_1,const float a_2):m_position(x,y,z),m_texture(a_1,a_2){}


  // Accessors / Mutators
    Q_DECL_CONSTEXPR const QVector3D& position() const;
    Q_DECL_CONSTEXPR const QVector3D& normal() const;
    Q_DECL_CONSTEXPR const QVector2D& texture() const;

    void setPosition(const QVector3D& position);
    void setColor (const float r,const float b,const float g);
    void setNormal(const QVector3D& normal);
    void setTexture(const QVector2D& texture);

  // OpenGL Helpers
    static const int PositionTupleSize = 3;
    static const int NormalTupleSize = 3;
    static const int TextureTupleSize = 2;
    static const int ColorTupleSize = 3;

    static Q_DECL_CONSTEXPR int positionOffset();
    static Q_DECL_CONSTEXPR int normalOffset();
    static Q_DECL_CONSTEXPR int colorOffset();
    static Q_DECL_CONSTEXPR int textureOffset();
    static Q_DECL_CONSTEXPR int stride();


private:
    QVector3D m_position;  //位置
    QVector3D m_color;    //颜色
    QVector3D m_normal;   //法向量

    QVector2D m_texture;   //纹理
};

/*******************************************************************************
 * Inline Implementation
 ******************************************************************************/

// Note: Q_MOVABLE_TYPE means it can be memcpy'd.
Q_DECLARE_TYPEINFO(Vertex, Q_MOVABLE_TYPE);

// Constructors

// Accessors / Mutators
Q_DECL_CONSTEXPR inline const QVector3D& Vertex::position() const { return m_position; }
Q_DECL_CONSTEXPR inline const QVector3D& Vertex::normal() const { return m_normal; }
Q_DECL_CONSTEXPR inline const QVector2D& Vertex::texture() const { return m_texture; }

void inline Vertex::setPosition(const QVector3D& position) { m_position = position; }
void inline Vertex::setNormal(const QVector3D& normal) { m_normal = normal; }
void inline Vertex::setTexture(const QVector2D& texture) { m_texture =texture; }
void inline Vertex::setColor (const float r,const float b,const float g){m_color=QVector3D(r,g,b);}

// OpenGL Helpers
Q_DECL_CONSTEXPR inline int Vertex::positionOffset() { return offsetof(Vertex, m_position); }
Q_DECL_CONSTEXPR inline int Vertex::normalOffset() { return offsetof(Vertex, m_normal); }
Q_DECL_CONSTEXPR inline int Vertex::textureOffset() { return offsetof(Vertex, m_texture); }
Q_DECL_CONSTEXPR inline int Vertex::colorOffset() { return offsetof(Vertex, m_color); }

Q_DECL_CONSTEXPR inline int Vertex::stride() { return sizeof(Vertex); }

#endif // VERTEX_H
