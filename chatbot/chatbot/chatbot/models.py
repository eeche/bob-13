from sqlalchemy import Column, ForeignKey, Integer, String, Float, DateTime
from sqlalchemy.orm import relationship
from sqlalchemy.sql import func
from database import Base

class Access_Table(Base): 
    __tablename__ = "access_table"

    id = Column(Integer, primary_key=True, index=True)
    user_id = Column(String(50))
    channel_id = Column(String(50))
    access_time = Column(DateTime)
    access_id = Column(String(50))

class User(Base):
    __tablename__ = "users"

    id = Column(Integer, primary_key=True, index=True)
    username = Column(String(50), unique=True, index=True)
    email = Column(String(50), unique=True, index=True)
    
    # iocs = relationship("IoC", back_populates="user")

class BoB(Base):
    __tablename__ = "bob"

    id = Column(Integer, primary_key=True, index=True)
    name = Column(String(50), unique=True, index=True)
    age = Column(Integer, index=True)
    track = Column(String(50), index=True)
    etc = Column(String(250), index=True)


# class IoC(Base):
#     __tablename__ = "iocs"

#     id = Column(Integer, primary_key=True, index=True)
#     type = Column(String(50))  # 예: IP, URL, File Hash 등
#     value = Column(String(255))  # 실제 IoC 값
#     description = Column(String(500))
#     confidence = Column(Float)
#     created_at = Column(DateTime(timezone=True), server_default=func.now())
#     user_id = Column(Integer, ForeignKey("users.id"))

#     user = relationship("User", back_populates="iocs")