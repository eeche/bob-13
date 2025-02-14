import schema
import crud
import logging
from fastapi import FastAPI, Depends, HTTPException
from sqlalchemy.orm import Session
from database import db
# from typing import List
# from fastapi.responses import HTMLResponse
# from fastapi.staticfiles import StaticFiles
# from pathlib import Path
# form schema import UserSchema, IoCSchema, AccessResponse

app = FastAPI()
# app.mount("/static", StaticFiles(directory="/app/chatbot/static"), name="static")

@app.get("/")
async def root():
    logging.info("Hello World")
    return {"message": "Hello World"}

@app.post("/ioc")
async def ioc(ioc: schema.IoCData):
    """IoC_Data API""" 
    return  crud.ioc_check(ioc)

@app.post("/access")
async def access(access_data: schema.AccessData, db: Session = Depends(db.get_session)):
    return crud.write_access_data(access_data, db)

@app.post("/bob")
async def bob(bob_data: schema.BoBData, db: Session = Depends(db.get_session)):
    return crud.write_bob_data(bob_data, db)

@app.post("/bob/search")
async def bob(bob_data: schema.BoBData, db: Session = Depends(db.get_session)):
    return crud.get_bob_data(bob_data, db)


# @app.post("/access", response_model=schema.AccessResponse)
# async def access(user_input: schema.UserData, db: Session = Depends(db.get_session)):

#     user = crud.get_user(db, user_input)
#     if not user:
#         raise HTTPException(status_code=404, detail="Sorry, user not found")
    
#     if user.email != user_input.email:
#         # crud.create_ioc(db, user, user_input, crud.IoCType.EMAIL_MISMATCH)
#         raise HTTPException(status_code=401, detail="Email does not match")
    
#     return schema.AccessResponse(
#         message=f"Welcome!! {user.username}",
#         user_id=user.id
#     )

# @app.get("/hi")
# async def hi():
#     logging.info("hi")
#     index_file = Path("./templates/index.html")
#     index_file = Path("/app/chatbot/tamplates/index.html")
#     with index_file.open() as file:
#         content = file.read()
#     return HTMLResponse(content=content)

# @app.get("/ioc/{user_id}", response_model=List[IoCSchema])
# async def get_ioc(user_id: int, db: Session = Depends(db.get_session)):
    
#     user = crud.get_user(db, user_id)
#     if not user:
#         raise HTTPException(status_code=404, detail="Sorry, user not found")
    
#     iocs = crud.get_iocs(db, user)
#     return [IoCSchema.model_validate(ioc) for ioc in iocs]