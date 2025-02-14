from typing import Optional, List
from pydantic import BaseModel, ConfigDict
from datetime import datetime

class AccessData(BaseModel):
    user_id: Optional[str] = None
    channel_id: Optional[str] = None
    access_time: Optional[datetime] = None
    access_id: Optional[str] = None

    model_config = ConfigDict(from_attributes=True)

class UserData(BaseModel):
    username: str
    email: str

    model_config = ConfigDict(from_attributes=True)

class IoCData(BaseModel):
    ioc_item: Optional[str] = None
    ioc_type: Optional[str] = None

    model_config = ConfigDict(from_attributes=True)

class AccessResponse(BaseModel):
    message: str
    user_id: int

    model_config = ConfigDict(from_attributes=True)

class BoBData(BaseModel):
    name: Optional[str] = None
    age: Optional[int] = None
    track: Optional[str] = None
    etc: Optional[str] = None
    
    model_config = ConfigDict(from_attributes=True)

# class IoCBase(BaseModel):
#     type: str
#     value: str
#     description: Optional[str] = None
#     confidence: Optional[float] = None
#     created_at: Optional[datetime] = None

#     model_config = ConfigDict(from_attributes=True)

# class IoCSchema(IoCBase):
#     id: int
#     user_id: int

#     model_config = ConfigDict(from_attributes=True)

# class UserSchema(BaseModel):
#     username: str
#     email: str

#     model_config = ConfigDict(from_attributes=True)
        
# # user를 get할 때 사용
# class UserResponse(UserSchema):
#     id: int
#     iocs: List[IoCSchema] = []

#     model_config = ConfigDict(from_attributes=True)

# class UserCreate(UserSchema):
#     pass

# class IoCCreate(IoCBase):
#     pass