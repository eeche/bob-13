// SPDX-License-Identifier: MIT
pragma solidity >= 0.7;

interface IERC20 {
    // 얼마를 발행할지, 잔고, 송금량
    function totalSupply() external view returns (uint256);
    function balanceOf(address account) external view returns (uint256);
    function transfer(address recipient, uint256 amount) external returns (bool);
    // 약정을 걸 때 얼마를 걸어줬는지, 승인하는 함수, 어디서 어디로 보낼지
    function allowance(address tokenOwner, address spender) external view returns (uint256);
    function approve(address spender, uint256 amount) external returns (bool);
    function transferFrom(address sender, address recipient, uint256 amount) external returns (bool);
    // 이벤트는 DApp(프론트엔드)을 만들었을 때 호출되는 이벤트

    event Transfer(address indexed from, address indexed to, uint256 value);
    event Approval(address indexed owner, address indexed spender, uint256 value);
}

contract BoBToken is IERC20 {
    string public name = "BoBToken";
    string public symbol = "BoB";
    uint8 public decimals = 1; // default 18;
    uint256 private _totalSupply;
    mapping(address => uint256) private _balances;
    mapping(address => mapping(address => uint256)) private _allowances;
    address public owner;

    modifier onlyOwner() {
        require(msg.sender == owner, "Caller is not the owner");
        _;
    }

    constructor(uint256 initialSupply) {
        owner = msg.sender;
        _totalSupply = initialSupply;   // 배포할 때 초기자금
        _balances[msg.sender] = _totalSupply;
        emit Transfer(address(0), msg.sender, _totalSupply);    // event 호출함수
    }

    function totalSupply() public view override returns (uint256) {
        return _totalSupply;
    }

    function balanceOf(address account) public view override returns (uint256) {
        return _balances[account];
    }

    function transfer(address recipient, uint256 amount) public override returns (bool) {
        require(_balances[msg.sender] >= amount, "Insufficient balance");
        _balances[msg.sender] -= amount;
        _balances[recipient] += amount;
        emit Transfer(msg.sender, recipient, amount);
        return true;
    }

    function allowance(address tokenOwner, address spender) public view 
    override returns (uint256) {
        return _allowances[tokenOwner][spender];
    }

    // 취약점 -> 30을 승인해줬을 때 15만 썼을 경우, 다시 30을 승인해주면 45가 안됨
    function approve(address spender, uint256 amount) public override returns (bool) {
        _allowances[msg.sender][spender] = amount;
        emit Approval(msg.sender, spender, amount);
        return true;
    }

    function transferFrom(address sender, address recipient, uint256 amount) public override returns (bool) {
        require(_balances[sender] >= amount, "Insufficient balance");
        require(_allowances[sender][msg.sender] >= amount, "Allowance exceeded");
        _balances[sender] -= amount;
        _balances[recipient] += amount;
        _allowances[sender][msg.sender] -= amount;
        emit Transfer(sender, recipient, amount);
        return true;
    }

    function burn(uint256 amount) public returns (bool) {
        require(_balances[msg.sender] >= amount, "Insufficient balance to burn");
        _balances[msg.sender] -= amount;
        _totalSupply -= amount;
        emit Transfer(msg.sender, address(0), amount);
        return true;
    }

    function mint(uint256 amount) public onlyOwner returns (bool) {
        uint256 mintAmount = amount;
        _totalSupply += mintAmount;
        _balances[owner] += mintAmount;
        emit Transfer(address(0), owner, mintAmount);
        return true;
    }
}