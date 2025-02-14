from datetime import datetime
from sqlalchemy.orm import Session
from vt import virustotal
import models
import schema
# from zoneinfo import ZoneInfo
# from enum import Enum
# from schema import UserSchema, IoCData
# from models import User, IoC

def get_user(db: Session, user_identifier):
    if isinstance(user_identifier, int):
        return db.query(models.User).filter(models.User.id == user_identifier).first()
    elif hasattr(user_identifier, 'username'):
        return db.query(models.User).filter(models.User.username == user_identifier.username).first()
    else:
        raise ValueError("Invalid user identifier")

def ioc_check(ioc: schema.IoCData):
    if ioc.ioc_type == "ip":
        result = virustotal(ioc.ioc_item, ioc.ioc_type)
        return result
    return {"message": "Invalid IoC type"}

def write_access_data(access_data: schema.AccessData, db: Session):
    access_entry = models.Access_Table(
        user_id = access_data.user_id,
        channel_id = access_data.channel_id,
        access_time = access_data.access_time,
        access_id = access_data.access_id
    )
    db.add(access_entry)
    db.commit()
    result = {
        "Access Granted": True,
    }
    return result

def write_bob_data(bob_data: schema.BoBData, db: Session):
    bob_entry = models.BoB(
        name = bob_data.name,
        age = bob_data.age,
        track = bob_data.track,
        etc = bob_data.etc
    )
    db.add(bob_entry)
    db.commit()
    result = {
        "BoB Data Added": True,
    }
    return result

def get_bob_data(bob_data: schema.BoBData, db: Session):
    query = db.query(models.BoB)
    if bob_data.name:
        query = query.filter(models.BoB.name == bob_data.name)
    if bob_data.age:
        query = query.filter(models.BoB.age == bob_data.age)
    if bob_data.track:
        query = query.filter(models.BoB.track == bob_data.track)
    return query.all()

# class IoCType(Enum):
#     EMAIL_MISMATCH = "Email Mismatch"
#     # PASSWORD_ATTEMPT = "Password Attempt"
#     # USERNAME_ATTEMPT = "Username Attempt"

# def get_iocs(db: Session, user: User):
#     return db.query(IoC).filter(IoC.user_id == user.id).all()

# # 다수의 ioc 생성 경우에 따라서 다르게 생성할 필요가 있음 ex) password, email, username 등
# def create_ioc(db: Session, user: User, user_input: UserSchema, ioc_type: IoCType):

#     if ioc_type == IoCType.EMAIL_MISMATCH:
#         value = user_input.email
#         description = f"Attempted access with incorrect email for user {user.username}"
#         confidence = 0.6
#     elif ioc_type == IoCType.PASSWORD_ATTEMPT:
#         value = user_input.password
#         description = f"Failed password attempt for user {user.username}"
#         confidence = 0.7
#     elif ioc_type == IoCType.USERNAME_ATTEMPT:
#         value = user_input.username
#         description = f"Attempted access with non-existent username"
#         confidence = 0.8
#     # 필요한 다른 IoC 타입들에 대한 로직을 여기에 추가

#     new_ioc = IoC(
#         type=ioc_type.value,
#         value=value,
#         description=description,
#         confidence=confidence,
#         user_id=user.id,
#         created_at=datetime.now(ZoneInfo("Asia/Seoul"))
#     )
#     db.add(new_ioc)
#     db.commit()
#     return new_ioc